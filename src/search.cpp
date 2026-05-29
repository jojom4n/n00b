#include "search.h"
#include "display.h"
#include "evaluation.h"
#include "makemove.h"
#include "movegen.h"
#include "moveorder.h"
#include "nnue_eval.h"
#include "pch.h"
#include "tt.h"
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>

thread_local struct Search search{};

const Move iterativeSearch(Position& p, Search& search, short const& depth)
{
    search.pos = p;
    search.depth = depth;
    search.height = 0;

    unsigned int totalTime{};

    std::vector<Move> moveList;
    moveList.reserve(MAX_PLY);
    moveList = moveGeneration(search.pos);
    pruneIllegal(moveList, search.pos);

    if (moveList.size() == 0)
    {

        if (!underCheck(search.pos.getTurn(), search.pos))
            std::cout << "\nIt's STALEMATE!" << std::endl;
        else if (underCheck(search.pos.getTurn(), search.pos))
        {
            search.pos.setCheckmate(true); // just in case position is examined for the first time
            std::cout << "\nIt's CHECKMATE!" << std::endl;
        }

        return 0;
    }

    else
    {

        for (short ply = 1; ply <= depth; ply++)
        {
            search.nodes = 0;
            search.ttHits = 0;
            search.ttUseful = 0;

            auto depthTimeStart = Clock::now();
            search.bestScore = newPVS<true>(p, search, ply, ALPHA, BETA, search.pv);
            auto depthTimeEnd = Clock::now();

            std::chrono::duration<float, std::milli> depthTime = depthTimeEnd - depthTimeStart;
            totalTime += static_cast<int>(depthTime.count());

            if (search.bestMove)
            {
                std::cout << "*depth:" << ply << " nodes:" << search.nodes
                          << " ms:" << (unsigned int)(depthTime.count())
                          << " total_ms:" << totalTime
                          << " nps:" << (unsigned int)(search.nodes / (depthTime.count() / 1000))
                          << " TT_hits:" << search.ttHits << " TT_useful:" << search.ttUseful
                          << "\n";

                std::cout << "\t move:" << displayMove(search.pos, search.bestMove) << " score:";

                if (!(search.bestScore == (MATE + search.height) &&
                      !(search.bestScore == -MATE - search.height)))
                {

                    float score = static_cast<float>(search.bestScore / 100.00);

                    switch (search.pos.getTurn())
                    {
                    case WHITE:
                        if (score > 0.0f)
                            std::cout << "+";
                        else if (score < 0.0f)
                            std::cout << "-";
                        break;
                    case BLACK:
                        if (score > 0.0f)
                            std::cout << "-";
                        else if (score < 0.0f)
                            std::cout << "+";
                        break;
                    default:
                        break;
                    }

                    std::cout << std::setprecision(3) << fabs(score);
                }
                else
                {
                    ushort index{};
                    for (index = 0; search.pv[index] != 0; index++)
                        ;
                    std::cout << "#" << (index / 2 + 1);
                }

                std::cout << " pv:";

                for (ushort i = 0; search.pv[i] != 0; i++)
                {
                    std::cout << displayMove(search.pos, search.pv[i]) << " ";
                }

                std::cout << std::endl;
            }

            else if (!underCheck(search.pos.getTurn(), search.pos))
            {
                std::cout << "\nIt's STALEMATE!" << std::endl;
                return 0;
            }

            else if (underCheck(search.pos.getTurn(), search.pos))
            {
                std::cout << "\nIt's CHECKMATE!" << std::endl;
                search.pos.setCheckmate(true);
                return 0;
            }
        }

        return search.bestMove;
    }
}

template <bool nullMove>
const short newPVS(Position& p, Search& search, short const& depth, short alpha, short beta,
                   Move* pv)
{
    const bool rootNode = (search.height == 0), isPV = (alpha != beta - 1);
    const short mateScore = -MATE + search.height;
    short bestScore = -MATE, score = -MATE, moveCount{}, played{};
    Move bestMove{}, subPV[MAX_PLY]{};
    auto alphaOrig = alpha; // Necessario per determinare il tipo di nodo (EXACT, UPPER, LOWER)

    if (depth <= 0)
        return quiescence(p, search, alpha, beta);

    const TTEntry* entryPtr =
        TT::Lookup(static_cast<uint32_t>(p.getZobrist()), static_cast<uint8_t>(depth));
    if (entryPtr)
    {
        search.ttHits++;
        const TTEntry& entry = *entryPtr;
        if (entry.nodeType == TTNodeType::EXACT)
        {
            search.ttUseful++;
            if (rootNode && entry.move)
                search.bestMove = entry.move;
            // In PV nodes, do NOT return early from TT.
            // Continue searching to build the full PV via recursive memcpy.
            if (!isPV)
                return entry.score;
        }
        else if (entry.nodeType == TTNodeType::LOWER)
        {
            alpha = std::max(alpha, entry.score);
        }
        else if (entry.nodeType == TTNodeType::UPPER)
        {
            beta = std::min(beta, entry.score);
        }
        if (!isPV && alpha >= beta)
        {
            search.ttUseful++;
            if (rootNode && entry.move)
                search.bestMove = entry.move;
            return entry.score;
        }
    }

    pv[0] = 0;

    if (!rootNode)
    {
        // MATE DISTANCE PRUNING
        if (mateScore > alpha)
        {
            alpha = mateScore;
            if (beta <= mateScore)
                return mateScore;
        }
    }

    short staticEval = eval_NNUE(p);
    initKillerMoves(search);

    // REVERSE FUTILITY PRUNING
    if (!isPV && !underCheck(p.getTurn(), p) && depth <= RFP_DEPTH &&
        staticEval - RFP_MARGIN * depth >= beta)
        return eval_NNUE(p);

    // ALPHA PRUNING
    if (!isPV && !underCheck(p.getTurn(), p) && depth <= ALPHA_PRUNING_DEPTH &&
        staticEval + ALPHA_PRUNING_MARGIN <= alpha)
        return eval_NNUE(p);

    // FUTILITY PRUNING
    if (!isPV && !underCheck(p.getTurn(), p) && depth == 1 && !p.isEnding() &&
        staticEval + MARGIN < alpha)
        return quiescence(p, search, alpha, beta);

    // EXTENDED FUTILITY PRUNING
    if (!isPV && !underCheck(p.getTurn(), p) && depth == 2 && !p.isEnding() &&
        staticEval + EXTENDED_MARGIN < alpha)
        return quiescence(p, search, alpha, beta);

    // NULL-MOVE PRUNING
    if (!isPV && !underCheck(p.getTurn(), p) && staticEval >= beta && depth >= NMP_DEPTH &&
        nullMove && !p.isEnding())
    {

        const short R = MAX_R + depth / (MIN_R * 2) + std::min(3, (staticEval - beta) / 200);
        doNullMove(depth, p);
        search.height++;
        short nullScore = -newPVS<false>(p, search, depth - R, -beta, -beta + 1, subPV);
        p.restoreState(depth);
        search.height--;

        if (nullScore >= beta)
            return beta;
    }

    std::vector<Move> moveList;
    moveList.reserve(MAX_PLY);
    moveList = moveGeneration(p);
    moveList = ordering(moveList, p, depth, search);

    for (const auto& m : moveList)
    {
        p.storeState(depth);

        if (doMove(m, p) == false)
        {
            undoMove(m, p);
            p.restoreState(depth);
            continue;
        }

        search.nodes++, played++, search.height++;

        if (isPV && played == 1)
            score = -newPVS<false>(p, search, depth - 1, -beta, -alpha, subPV);
        else
        {
            score = -newPVS<true>(p, search, depth - 1, -alpha - 1, -alpha, subPV);

            if (score > alpha && score < beta)
                score = -newPVS<false>(p, search, depth - 1, -beta, -alpha, subPV);
        }

        undoMove(m, p);
        search.height--;
        p.restoreState(depth);

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = m;

            if (score > alpha)
            {
                alpha = score;
                pv[0] = bestMove;
                memcpy(pv + 1, subPV, 63 * sizeof(Move));
                pv[63] = 0;

                if (alpha >= beta)
                    break;
            }
        }
    }

    if (played == 0)
    {
        search.bestMove = bestMove;
        return underCheck(p.getTurn(), p) ? -MATE + search.height : 0;
    }

    // --- INTEGRAZIONE TT: Salvataggio del nodo ---
    TT::Store(
        static_cast<uint32_t>(p.getZobrist()), static_cast<uint8_t>(depth), bestMove, bestScore,
        static_cast<TTNodeType>(bestScore <= alphaOrig
                                    ? TTNodeType::UPPER
                                    : (bestScore >= beta ? TTNodeType::LOWER : TTNodeType::EXACT)),
        static_cast<uint8_t>(p.getMoveNumber()));

    search.bestMove = bestMove;
    return bestScore;
}

const short quiescence(Position& p, Search& search, short alpha, short beta)
{
    short stand_pat = eval_NNUE(p);

    if (stand_pat >= beta)
        return beta;

    if (stand_pat < alpha - g_pieceValue.at(QUEEN)) // Delta pruning
        return alpha;

    if (alpha < stand_pat)
        alpha = stand_pat;

    std::vector<Move> moveList;
    moveList.reserve(MAX_PLY);
    moveList = moveGenQS(p);
    pruneIllegal(moveList, p);

    if (moveList.size() > 0)
        moveList = mvv_lva(moveList);

    for (const auto& m : moveList)
    {
        p.storeState();
        doMove(m, p);
        search.nodes++;
        short score = -quiescence(p, search, -beta, -alpha);
        undoMove(m, p);
        p.restoreState();

        if (score >= beta)
            return beta;

        if (score > alpha)
            alpha = score;
    }

    return alpha;
}

void initKillerMoves(Search& search)
{
    search.killerMoves[search.height + 1][0] = 0;
    search.killerMoves[search.height + 1][1] = 0;
    search.killerMoves[search.height + 1][2] = 0;
}

