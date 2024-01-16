#include <cs50.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
                preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int i = 0;
    pair_count = 0;
        for (int m = 0; m < candidate_count; m++)
        {
            for (int n = 0; n < candidate_count; n++)
            {
                if (preferences[m][n] > preferences[n][m])
                {
                    pairs[i].winner = m;
                    pairs[i].loser = n;
                    pair_count++;
                    i++;
                }
            }
        }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    for (int i = 0; i < pair_count - 1; i++)
    {
        for (int j = 0; j <= i - 1; j++)
        {
            if ((preferences[pairs[j].winner][pairs[j].loser]) < (preferences[pairs[j + 1].winner][pairs[j + 1].loser]))
            {
                pair temp = pairs[j];
                pairs[j] = pairs[j + i];
                pairs[j + 1] = temp;
            }
        }
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles

// a.1 Adjacency list
void lock_pairs(void)
{
    int size = MAX * (MAX - 1);
    int j = 0;
    pair NEW[size];

    for (int n = 0; n < pair_count; n++)
    {
        int flag = 0;
        for (int i = 0; i <= j; i++)
        {
            if (pairs[n].winner == NEW[i].loser && pairs[n].loser == NEW[i].winner)
            {
                flag = 1;
                break;
            }
        }
        if (flag != 1)
        {
            locked[pairs[n].winner][pairs[n].loser] = true;

            NEW[j].winner = pairs[n].winner;
            NEW[j].loser = pairs[n].loser;
            int achor = j;
            j++;

            for (int m = achor; m < j; m++)
            {
                for (int k = 0; k < achor; k++)
                {
                    if (NEW[m].winner == NEW[k].loser)
                    {
                        for (int a = 0; a < achor; a++)
                        {
                            if (NEW[a].winner == NEW[k].winner && NEW[a].loser == NEW[m].loser)
                            {
                                flag = 1;
                                break;
                            }
                        }
                        if (flag != 1)
                        {
                            NEW[j].winner = NEW[k].winner;
                            NEW[j].loser = NEW[m].loser;
                            j++;
                            flag = 0;
                        }
                    }
                }
            }
        }
    }
    return;
}

// a.2 Adjacency matrix
void lock_pairs(void)
{
    bool NEW[MAX][MAX] = {{}};
    bool (*NEW)[candidate_count] = calloc(candidate_count, sizeof *NEW);

    for (int i = 0; i < pair_count; i++)
    {
        if (!NEW[pairs[i].loser][pairs[i].winner])
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
            NEW[pairs[i].winner][pairs[i].loser] = true;
            for (int j = 0; j < candidate_count; j++)
            {
                NEW[j][pairs[i].loser] = NEW[j][pairs[i].winner] || NEW[j][pairs[i].loser];
            }
        }
    }
    return;
}
// a.3 Bitwise operations 
void lock_pairs(void)
{
    int NEW[MAX] = {};
    for (int i = 0; i < pair_count; i++)
    {
        if (!(NEW[pairs[i].winner] & (1 << pairs[i].loser)))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
            NEW[pairs[i].loser] |= ((1 << pairs[i].winner) | NEW[pairs[i].winner]);
        }
    }
}

// b. Print the winner of the election
void print_winner(void)
{
    for (int j = 0; j < candidate_count; j++)
    {
        bool is_winner = true;
        for (int i = 0; i < candidate_count; i++)
        {
            if (locked[i][j])
            {
                is_winner = false;
                break;
            }
        }
        if (is_winner)
        {
            printf("%s\n", candidates[j]);
        }
    }
    return;
}

// c. Using bitwise operations to lock pairs and print winners
void find_winner(void)
{
    int NEW[MAX] = {};
    for (int i = 0; i < pair_count; i++)
    {
        if (!(NEW[pairs[i].winner] & (1 << pairs[i].loser)))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
            NEW[pairs[i].loser] |= ((1 << pairs[i].winner) | NEW[pairs[i].winner]);
        }
    }
    for (int j = 0; j < candidate_count; j++)
    {
        if (!NEW[j]) {
            printf("%s\n", candidates[j]);
        }
    }
}



