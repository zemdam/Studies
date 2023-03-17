/*
JNP1 - 2022/23
Zadanie 1
Michał Płachta, Adam Zembrzuski
*/

#include <iostream>
#include <regex>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::all_of;
using std::cerr;
using std::cin;
using std::cout;
using std::getline;
using std::make_tuple;
using std::pair;
using std::prev;
using std::regex;
using std::regex_match;
using std::set;
using std::sregex_iterator;
using std::stoi;
using std::string;
using std::to_string;
using std::tuple;
using std::unordered_map;
using std::unordered_set;
using std::vector;

//// arg0 - vote index, arg1 - number of votes/points
using VotePair = pair<uint32_t, size_t>;

struct voteComparator {
    bool operator()(VotePair const pair1, VotePair const pair2) const {
        return pair1.second > pair2.second ||
               (pair1.second == pair2.second && pair1.first < pair2.first);
    }
};

//// arg0 - vote index, arg1 - place in ranking
using Ranking = unordered_map<uint32_t, uint32_t>;
using VoteHistory = vector<Ranking>;
using VoteSet = set<VotePair, voteComparator>;
//// arg0 - vote index
using CurrentVoting = unordered_map<uint32_t, VotePair>;
//// arg0 - vote index
using OutOfRanking = unordered_set<uint32_t>;
using VoteVector = vector<VotePair>;
//// arg3 - vote index
using VoteTuple = tuple<CurrentVoting, VoteHistory, VoteHistory, uint32_t,
                        OutOfRanking, uint32_t, CurrentVoting>;

static const uint MAX_PLACE_NUM = 7;
static const uint WRONG_LINE = 0;
static const uint LINE_TOP = 1;
static const uint LINE_NEW = 2;
static const uint LINE_VOTES = 3;
static const uint LINE_EMPTY = 4;
static const uint CURRENT_VOTING = 0;
static const uint VOTE_NEW_HISTORY = 1;
static const uint VOTE_TOP_HISTORY = 2;
static const uint MAX = 3;
static const uint OUT_OF_RANKING = 4;
static const uint LAST_TOP = 5;
static const uint TOP_VOTE = 6;

namespace {
    regex const voteRegex("[0-9]+");

    bool isEmptyWithWhitespaces(string const &line) {
        return line.empty() || all_of(line.begin(), line.end(), isspace);
    }

    void putInHistory(VoteVector &currentVoting, VoteHistory &voteHistory,
                      size_t length) {
        Ranking &ranking = voteHistory.back();
        for (size_t i = 0; i < length; i++) {
            ranking.insert({currentVoting.at(i).first, i});
        }
    }

    string checkPositionChange(VoteHistory &voteHistory, uint32_t index,
                               uint32_t position) {
        if (voteHistory.empty()) {
            return "-";
        }

        if (voteHistory.back().count(index)) {
            int shift = (int) (voteHistory.back()[index] - position);
            return to_string(shift);
        }

        return "-";
    }

    bool isOutOfRanking(uint32_t index, OutOfRanking &outOfRanking) {
        return outOfRanking.count(index);
    }

    VoteVector topSeven(CurrentVoting &currentVoting,
                        OutOfRanking &outOfRanking) {
        VoteSet voteSet;
        CurrentVoting tmpCurrentVoting = currentVoting;
        for (auto iterator: tmpCurrentVoting) {
            voteSet.insert(iterator.second);
            if (voteSet.size() > MAX_PLACE_NUM) {
                VotePair toDelete = *(voteSet.rbegin());
                if (isOutOfRanking(toDelete.first, outOfRanking)) {
                    currentVoting.erase(toDelete.first);
                }
                voteSet.erase(prev(voteSet.end()));
            }
        }

        VoteVector voteVector(voteSet.begin(), voteSet.end());
        return voteVector;
    }

    void printQuoting(VoteHistory &voteHistory, CurrentVoting &currentVoting,
                      OutOfRanking &outOfRanking) {
        VoteVector voteVector = topSeven(currentVoting, outOfRanking);

        for (size_t i = 0; i < voteVector.size(); i++) {
            size_t index = voteVector.at(i).first;
            cout << index + 1 << " "
                 << checkPositionChange(voteHistory, index, i) << "\n";
        }

        Ranking ranking;
        voteHistory.push_back(ranking);
        putInHistory(voteVector, voteHistory, voteVector.size());
    }

    void generateTopVotes(VoteTuple &voteTuple) {
        VoteHistory &multiVotes = get<VOTE_NEW_HISTORY>(voteTuple);
        CurrentVoting &currentVoting = get<TOP_VOTE>(voteTuple);

        for (size_t i = get<LAST_TOP>(voteTuple); i < multiVotes.size(); i++) {
            for (auto &iterator: multiVotes.at(i)) {
                uint32_t points = MAX_PLACE_NUM - iterator.second;
                if (currentVoting.count(iterator.first)) {
                    currentVoting.at(iterator.first).second += points;
                } else {
                    VotePair votePair;
                    votePair.second = points;
                    votePair.first = iterator.first;
                    currentVoting.insert({iterator.first, votePair});
                }
            }
        }

        get<LAST_TOP>(voteTuple) = multiVotes.size();
        printQuoting(get<VOTE_TOP_HISTORY>(voteTuple), currentVoting,
                     get<OUT_OF_RANKING>(voteTuple));
    }

    void printError(string const &line, size_t lineNum) {
        cerr << "Error in line " << lineNum << ": " << line << "\n";
    }

    void checkRanking(VoteTuple &voteTuple) {
        VoteHistory &voteHistory = get<VOTE_NEW_HISTORY>(voteTuple);
        OutOfRanking &outOfRanking = get<OUT_OF_RANKING>(voteTuple);
        if (voteHistory.size() < 2) {
            return;
        }

        Ranking recentRanking = voteHistory.back();
        Ranking lastRanking = voteHistory[voteHistory.size() - 2];
        for (auto iterator: lastRanking) {
            if (!recentRanking.count(iterator.first)) {
                outOfRanking.insert(iterator.first);
            }
        }
    }

    void createNewVoting(string const &line, size_t line_num,
                         VoteTuple &voteTuple) {
        printQuoting(get<VOTE_NEW_HISTORY>(voteTuple),
                     get<CURRENT_VOTING>(voteTuple),
                     get<OUT_OF_RANKING>(voteTuple));
        checkRanking(voteTuple);
        auto voteIterator =
                sregex_iterator(line.begin(), line.end(), voteRegex);
        auto endIterator = sregex_iterator();
        auto &historyNew = get<VOTE_NEW_HISTORY>(voteTuple);
        uint32_t newMax = stoi(voteIterator->str());
        get<MAX>(voteTuple) = newMax;

        if (!historyNew.empty() && historyNew.back().size() > newMax) {
            printError(line, line_num);
            return;
        }

        CurrentVoting newCurrentVoting;
        get<CURRENT_VOTING>(voteTuple) = newCurrentVoting;
    }

    void markVotes(string const &line, size_t line_num, VoteTuple &voteTuple) {
        auto voteIterator =
                sregex_iterator(line.begin(), line.end(), voteRegex);
        auto endIterator = sregex_iterator();
        CurrentVoting &currentVoting = get<CURRENT_VOTING>(voteTuple);
        size_t max = get<MAX>(voteTuple);
        unordered_set<uint32_t> votesSet;

        while (voteIterator != endIterator) {
            uint32_t newVoteValue = stoi((voteIterator++)->str()) - 1;

            if (newVoteValue >= max ||
                isOutOfRanking(newVoteValue, get<OUT_OF_RANKING>(voteTuple)) ||
                !votesSet.insert(newVoteValue).second) {
                printError(line, line_num);
                return;
            }
        }

        for (auto iterator: votesSet) {
            if (currentVoting.count(iterator)) {
                currentVoting.at(iterator).second++;
            } else {
                VotePair votePair;
                votePair.first = iterator;
                votePair.second = 1;
                currentVoting.insert({iterator, votePair});
            }
        }
    }

    int checkLine(string const &line) {
        static regex const validLineNew(
                R"(\s*NEW\s+(0*[1-9]|0*[1-9][0-9]{1,7})\s*)");
        static regex const validLineTop("\\s*TOP\\s*");
        static regex const validLineVotes(
                R"(\s*(0*[1-9]|0*[1-9][0-9]{1,7})(\s+(0*[1-9]|0*[1-9][0-9]{1,7}))*\s*)");

        if (isEmptyWithWhitespaces(line)) {
            return LINE_EMPTY;
        }

        if (regex_match(line, validLineTop)) {
            return LINE_TOP;
        }

        if (regex_match(line, validLineNew)) {
            return LINE_NEW;
        }

        if (regex_match(line, validLineVotes)) {
            return LINE_VOTES;
        }

        return WRONG_LINE;
    }
} // namespace

int main() {
    VoteHistory historyNew;
    VoteHistory historyTop;
    CurrentVoting currentVoting;
    uint32_t max = 0;
    uint32_t lastTop = 0;
    CurrentVoting topVoting;
    OutOfRanking outOfRanking;
    VoteTuple voteTuple = make_tuple(currentVoting, historyNew, historyTop, max,
                                     outOfRanking, lastTop, topVoting);
    size_t lineNum = 0;
    string line;

    while (getline(cin, line)) {
        lineNum++;
        switch (checkLine(line)) {
            case WRONG_LINE:
                printError(line, lineNum);
                break;
            case LINE_TOP:
                generateTopVotes(voteTuple);
                break;
            case LINE_NEW:
                createNewVoting(line, lineNum, voteTuple);
                break;
            case LINE_VOTES:
                markVotes(line, lineNum, voteTuple);
                break;
        }
    }

    return 0;
}