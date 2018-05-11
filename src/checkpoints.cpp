// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 5000;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (3, uint256("0xe22508af01ea949329b4e25f8d8e896956666b04847bf478db8a826263dca48c"))
        (11, uint256("0x1475396f1f43145f18b0aaedb867b8722a46d70affbacab3b6f75fc7b8233a8a"))
        (30, uint256("0x6538d7cae5d9fdb0655a102b86155bebc43851e4db3cc5f4d4f041bc305d36da"))
        (50, uint256("0x4a667533686c0ccf45b475a2e333bcbeebf3a3002e8a9597e381705544b9b7ed"))
        (75, uint256("0x17ff641bf5b6c7214d2e69a61843ce79c43791e0a2e233ed44053d1cd35d83f2"))
        (100, uint256("0xf59c6d8ea03b34031b79fb33c9b5a1b136697f643517bd869c4d5947cc95ded6"))
        (150, uint256("0x88be01e90a75462e6d01bcccba0c795743b75b66c9113da2bc0465f740040dff"))
        (225, uint256("0xa4490abaae285ee03c190a883ae454574dc15275d154b13c0c61bd05d0be1f52"))
        (300, uint256("0x34107e5aff3a8c28870ff60db083875d49f0266190ff92b8713aa738e98dac36"))
        (398, uint256("0x7aa2e020017ea908ecba0a97a180493272abcd1e92ebfeca98d13aaa34b97de8"))
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
