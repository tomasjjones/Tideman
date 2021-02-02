#include <cs50.h>
#include <stdio.h>
#include <string.h>

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
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;
//int num_of_pairs;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
void print_array_table(int vote_num);
bool makes_circle(int cycle_start, int loser);


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
// Update ranks given a new vote -- COMPLETED
bool vote(int rank, string name, int ranks[])
{
    // rank is the position this vote is in their list 0 = 1st, 1 = 2nd, 2 = 3rd
    // name is the name of their current vote in position rank
    // ranks[] is the persons voting slip rank 1 to 3

    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            //printf("Match Found rank: %i \nname: %s \n\n",rank, name);
            ranks[rank] = i;
            return true;
        }
    }
    // Catch all
    return false;
}


// Update preferences given one voter's ranks -- COMPLETED
void record_preferences(int ranks[])
{
    // preferences[i][j] is number of voters who prefer i over j
    // preferences[MAX][MAX];
    //for each entry in vote slip
    for (int i = 0; i < candidate_count - 1; i++)
    {
        //for each entry to the right of current vote
        for (int j = i; j < candidate_count - 1; j++)
        {
            preferences[ranks[i]][ranks[j + 1]]++;
        }
    }

    return;

}


// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // for each pair in array table check if there is a winner and loser and add to pairs array
    for (int i = 0; i < candidate_count; i++)
    {    
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
        
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        
            else //its a tie
            {}
        }
    }
    
    printf("Total pair results: %i\n", pair_count);
    
    //set winner loser vars
    int w, l;
    
    for (int count = 0; count < pair_count; count++)
    {   
        w = pairs[count].winner;
        l = pairs[count].loser;
        
        //print results for user
        printf("Pair %i: %i > %i  ||  Score: %i vs %i  ||  %s > %s\n", 
               count + 1, w, l, preferences[w][l], preferences[l][w], candidates[w], candidates[l]);
    }
    
    printf("\n");
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    //set variableS 
    int w, l, temp_diff, temp_biggest_check = 0, temp_biggest_pair_num, temp_winner, temp_loser;
    bool higher_found;
    
    //cycle through all 
    for (int count = 0; count < pair_count - 1; count++)
    {   
        //if no higher found break early
        higher_found = false;
        temp_biggest_check = 0;
        
        // find largest difference in array - 1
        for (int i = count; i < pair_count; i++)
        {
            w = pairs[i].winner;
            l = pairs[i].loser;
            temp_diff = preferences[w][l] - preferences[l][w];
                
            if (temp_diff > temp_biggest_check)
            {
                temp_biggest_check = temp_diff;
                temp_biggest_pair_num = i;
                higher_found = true;
            } 
        }
        
        if (higher_found == true)
        {
            //set temp before switch  
            temp_winner = pairs[count].winner;
            temp_loser = pairs[count].loser;
            
            pairs[count].winner = pairs[temp_biggest_pair_num].winner;
            pairs[count].loser = pairs[temp_biggest_pair_num].loser;

            pairs[temp_biggest_pair_num].winner = temp_winner;
            pairs[temp_biggest_pair_num].loser = temp_loser;
        }
        
        else 
        { 
            break; 
        }
    }
    
    
    //printf("New Order %i sorts: \n", count + 1);
    printf("sorted order: \n");
    for (int j = 0; j < pair_count; j++)
    {
        w = pairs[j].winner;
        l = pairs[j].loser;
        printf("%i > %i || %s > %s\n", preferences[w][l], preferences[l][w], candidates[w], candidates[l]);
    }
    
    printf("\n");
    return;
}


// Lock pairs to create true false matrix graph
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        int cycle_start, loser;
        
        cycle_start = pairs[i].winner;
        loser = pairs[i].loser;

        if ((makes_circle(cycle_start, loser)) == false)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;

        }
    }
    
    printf("Locked Pairs Table:\n");
    for (int r = 0; r < candidate_count; r++)
    {
        for (int c = 0; c < candidate_count; c++)
        {   
            printf("%s  ", locked[r][c] ? "true" : "false");
        }
            
        printf("\n");
    }
        
    printf("\n");
    
    return;
}

bool makes_circle(int cycle_start, int loser)
{
    //check to see if circle is broken by adding a line
    
    if (cycle_start == loser)
    {
        //If current loser is cycle start it must make a circle
        return true;
    }
    
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[loser][i] == true)
        {
            if (makes_circle(cycle_start, i))
            {
                // Forward progress through the circle
                return true;
            }
        }
    }
    return false;
}


// Print the winner of the election
void print_winner(void)
{
    // find the column in the table that all equals false 
    // meaning they lost to no one
    for (int c = 0; c < candidate_count; c++)
    {
        bool found_source = true;
        for (int r = 0; r < candidate_count; r++)
        {   
            if (locked[r][c] == true)
            {
                found_source = false;
                break;
            }
        }
     
        if (found_source == true)
        {
            printf("Winner: %s\n", candidates[c]);
        }
        
    }    
}
