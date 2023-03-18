// Implementation of the FriendBook ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Fb.h"
#include "Map.h"
#include "Queue.h"

#define DEFAULT_CAPACITY 1 // !!! DO NOT CHANGE THIS !!!

struct fb {
    int    numPeople;
    int    capacity;

    char **names;    // the id of a person is simply the index
                     // that contains their name in this array
    
    Map    nameToId; // maps names to ids

    bool **friends;
};

static void  increaseCapacity(Fb fb);
static char *myStrdup(char *s);
static int   nameToId(Fb fb, char *name);

////////////////////////////////////////////////////////////////////////
int do_FbFriendRecs1(Fb fb, int id1, int id2);
// Creates a new instance of FriendBook
Fb   FbNew(void) {
    Fb fb = malloc(sizeof(*fb));
    if (fb == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    fb->numPeople = 0;
    fb->capacity = DEFAULT_CAPACITY;
    
    fb->names = calloc(fb->capacity, sizeof(char *));
    if (fb->names == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    fb->nameToId = MapNew();

    fb->friends = malloc(fb->capacity * sizeof(bool *));
    if (fb->friends == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < fb->capacity; i++) {
        fb->friends[i] = calloc(fb->capacity, sizeof(bool));
        if (fb->friends[i] == NULL) {
            fprintf(stderr, "error: out of memory\n");
            exit(EXIT_FAILURE);
        }
    }

    return fb;
}

void FbFree(Fb fb) {
    for (int i = 0; i < fb->capacity; i++) {
        free(fb->friends[i]);
    }
    free(fb->friends);

    MapFree(fb->nameToId);

    for (int i = 0; i < fb->numPeople; i++) {
        free(fb->names[i]);
    }
    free(fb->names);
    
    free(fb);
}

bool FbAddPerson(Fb fb, char *name) {
    if (fb->numPeople == fb->capacity) {
        increaseCapacity(fb);
    }

    if (!MapContains(fb->nameToId, name)) {
        int id = fb->numPeople++;
        fb->names[id] = myStrdup(name);
        MapSet(fb->nameToId, name, id);
        return true;
    } else {
        return false;
    }
}

static void increaseCapacity(Fb fb) {
    int newCapacity = fb->capacity * 2;
    
    fb->names = realloc(fb->names, newCapacity * sizeof(char *));
    if (fb->names == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    for (int i = fb->capacity; i < newCapacity; i++) {
        fb->names[i] = NULL;
    }
    
    fb->friends = realloc(fb->friends, newCapacity * sizeof(bool *));
    if (fb->friends == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < fb->capacity; i++) {
        fb->friends[i] = realloc(fb->friends[i], newCapacity * sizeof(bool));
        if (fb->friends[i] == NULL) {
            fprintf(stderr, "error: out of memory\n");
            exit(EXIT_FAILURE);
        }
        for (int j = fb->capacity; j < newCapacity; j++) {
            fb->friends[i][j] = false;
        }
    }
    for (int i = fb->capacity; i < newCapacity; i++) {
        fb->friends[i] = calloc(newCapacity, sizeof(bool));
        if (fb->friends[i] == NULL) {
            fprintf(stderr, "error: out of memory\n");
            exit(EXIT_FAILURE);
        }
    }
    
    fb->capacity = newCapacity;
}

bool FbHasPerson(Fb fb, char *name) {
    return MapContains(fb->nameToId, name);
}

List FbGetPeople(Fb fb) {
    List l = ListNew();
    for (int id = 0; id < fb->numPeople; id++) {
        ListAppend(l, fb->names[id]);
    }
    return l;
}

bool FbFriend(Fb fb, char *name1, char *name2) {
    int id1 = nameToId(fb, name1);
    int id2 = nameToId(fb, name2);
    assert(id1 != id2);

    if (!fb->friends[id1][id2]) {
        fb->friends[id1][id2] = true;
        fb->friends[id2][id1] = true;
        return true;
    } else {
        return false;
    }
}

bool FbIsFriend(Fb fb, char *name1, char *name2) {
    int id1 = nameToId(fb, name1);
    int id2 = nameToId(fb, name2);
    return fb->friends[id1][id2];
}

int  FbNumFriends(Fb fb, char *name) {
    int id1 = nameToId(fb, name);
    
    int numFriends = 0;
    for (int id2 = 0; id2 < fb->numPeople; id2++) {
        if (fb->friends[id1][id2]) {
            numFriends++;
        }
    }
    return numFriends;
}

////////////////////////////////////////////////////////////////////////
// Your tasks

bool FbUnfriend(Fb fb, char *name1, char *name2) {
    int id1 = nameToId(fb, name1);
    int id2 = nameToId(fb, name2);
    // assert 1 and 2 not the same person.
    assert(id1 != id2);
    // if 1 and 2 are friends just delete.
    if (fb->friends[id1][id2]) {
        fb->friends[id1][id2] = false;
        fb->friends[id2][id1] = false;
        return true;
    } else {
        return false;
    }
}

List FbMutualFriends(Fb fb, char *name1, char *name2) {
    // assert we have 1 and 2.
    if(!FbHasPerson(fb, name1) || !FbHasPerson(fb, name2)) {
        return NULL;
    }
    List l = ListNew();
    bool check1 = false;
    bool check2 = false;
    for(int i = 0; i < fb->numPeople; i++) {
        check1 = FbIsFriend(fb, name1, fb->names[i]);
        check2 = FbIsFriend(fb, name2, fb->names[i]);
        // If someone is both 1 and 2 friends, they are mutual friends.
        if(check1 && check2) {
            ListAppend(l, fb -> names[i]);
        }
    }
    return l;
}

void FbFriendRecs1(Fb fb, char *name) {
    printf("%s's friend recommendations\n", name);
    // assert we have this person.
    if(!FbHasPerson(fb, name)) {
        return;
    }
    int id = nameToId(fb, name);
    // the maximum mutual friends are n - 2; and mini is 1.
    for (int num = fb -> numPeople -2; num > 0; num--) {
        int count = 0;
        for (int i = 0; i < fb -> numPeople; i++) {
            // assert not the same person and not friend.
            if(i != id && !fb->friends[id][i]) {
                // count mutual friends
                count = do_FbFriendRecs1(fb, id, i);
            }
            // its max and print.
            if (count == num) {
                printf("\t%-20s%4d mutual friends\n", fb -> names[i], num);
            }
        }
    }
}
// count person 1 and 2 mutual friends number.
int do_FbFriendRecs1(Fb fb, int id1, int id2) {
    assert(!fb -> friends[id1][id2]);
    int count = 0;
    for (int i = 0; i < fb -> numPeople; i++) {
        if(fb -> friends[id1][i] && fb -> friends[id2][i]) {
            count++;
        }
    }
    return count;
}
////////////////////////////////////////////////////////////////////////
// Optional task

void FbFriendRecs2(Fb fb, char *name) {
    // TODO: Complete this function
}

////////////////////////////////////////////////////////////////////////
// Helper Functions

static char *myStrdup(char *s) {
    char *copy = malloc((strlen(s) + 1) * sizeof(char));
    if (copy == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    return strcpy(copy, s);
}

// Converts a name to an ID. Raises an error if the name doesn't exist.
static int nameToId(Fb fb, char *name) {
    if (!MapContains(fb->nameToId, name)) {
        fprintf(stderr, "error: person '%s' does not exist!\n", name);
        exit(EXIT_FAILURE);
    }
    return MapGet(fb->nameToId, name);
}

