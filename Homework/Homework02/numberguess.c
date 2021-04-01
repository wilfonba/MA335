#include <stdio.h>
#include <stdlib.h>

int askIfCorrect(int guess)
{
    char line[100];
    char userInput;
    printf("Is your number %d? (Y/N) \n",guess);
    fgets(line,100,stdin);
    sscanf(line,"%s",&userInput);
    switch (userInput)
    {
        case 'Y':
            printf("Great, bye! \n");
            return 0;
        case 'y':
            printf("Great, bye! \n");
            return 0;
        case 'N':
            printf("Too bad. \n");
            return 1;
        case 'n':
            printf("Too bad. \n");
            return 1;
        default:
            printf("You didn't enter a proper response.  Please respond Y,y,N,or n \n");
            return askIfCorrect(guess);
    }
}

int askHighLow(int guess)
{
    char line[100];
    char userInput;
    printf("Is my guess too high (H) or too low (L)? \n");
    fgets(line,100,stdin);
    sscanf(line,"%s",&userInput);
    switch (userInput)
    {
        case 'H':
            printf("Ok, my guess was too high. \n");
            return 0;
        case 'h':
            printf("Ok, my guess was too high. \n");
            return 0;
        case 'L':
            printf("Ok, my guess was too low. \n");
            return 1;
        case 'l':
            printf("Ok, my guess was too low. \n");
            return 1;
        default:
            printf("You didn't enter a proper response.  Please respond H,h,L, or l \n");
            return askHighLow(guess);
    }
}

int main()
{
    printf("Welcome to the wonderful world of number guessing!\n");
    printf("Think of an integer number between 1 and 100.\n");
    
    int guess = 0;
    int guessing = 1;
    int highLow = -1;
    int bounds[2] = {1,100};
    while (guessing == 1)
    {
        guess = (bounds[0]+bounds[1])/2;
        if (guess == bounds[1] || guess == bounds[1])
        {
            //guess = (bounds[0]+bounds[1])/2;
            //printf("\n\nBounds 0 = %d Bounds 1 = %d Guess = %d\n\n\n",bounds[0],bounds[1],guess);
            //guessing = askIfCorrect(guess);
            //highLow = askHighLow(guess);
            printf("Liar! You Cheat!\n");
            break;
        }
        guessing = askIfCorrect(guess);
        if (guessing == 1)
        {
            //printf("\n\nBounds 0 = %d Bounds 1 = %d Guess = %d\n\n\n",bounds[0],bounds[1],guess);
            highLow = askHighLow(guess);
            if (highLow == 1)
            {
               bounds[0] = guess + 1;
            }
            if (highLow == 0)
            {
                bounds[1] = guess - 1;
            }
        }
    }
    return 0;
}