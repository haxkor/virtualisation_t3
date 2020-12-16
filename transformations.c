
#define MAX_INSTRLEN 8                                                                 

#define ADD6_INDEX 6
#define ADD9_INDEX 7
#define ADD2_INDEX 8
#define SETA3_INDEX 9
#define SETAM1_INDEX 10
#define CLRBOTH_INDEX 11

struct trans {                                                                        
    char code [MAX_INSTRLEN];                                                         
    int len;                                                                       
};

struct trans transformations[]= {
{{144}, 1},
{{49, 192}, 2},
{{131, 192, 3}, 3},
{{255, 200}, 2},
{{137, 216}, 2},
{{144}, 1},
{{131, 192, 6}, 3},
{{131, 192, 9}, 3},
{{131, 192, 2}, 3},
{{184, 3, 0, 0, 0}, 5},
{{184, 255, 255, 255, 255}, 5},
{{49, 192, 49, 219}, 4},};
struct trans prolog = {{85, 72, 137, 229, 137, 248, 137, 243}, 8};
struct trans epilog = {{93, 195}, 2};
