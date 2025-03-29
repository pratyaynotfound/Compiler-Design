#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LENGTH 10

const char *tokens[] = {"aaa", "aabba", "baab", "bbaba", "abb", "aab", "ab", "a", "b"};
const int num_tokens = 9;

// Function to find the longest valid token
int find_longest_token(const char *input, char *token) {
    int len = strlen(input);
    for (int i = 0; i < num_tokens; i++) {
        int token_len = strlen(tokens[i]);
        if (token_len <= len && strncmp(input, tokens[i], token_len) == 0) {
            strcpy(token, tokens[i]);
            return token_len;
        }
    }
    return 0;
}

void tokenize_input(const char *input) {
    char token[MAX_TOKEN_LENGTH];
    int i = 0, len = strlen(input);

    while (i < len) {
        if (input[i] == ' ') {
            i++;
            continue;
        }
        int token_len = find_longest_token(input + i, token);
        if (token_len > 0) {
            printf("< %s > ", token);
            i += token_len;
        } else {
            i++; //if not a valid token
        }
    }
    printf("\n");
}

int main() {
    char input[256];

    printf("Enter input string: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    tokenize_input(input);
    return 0;
}
