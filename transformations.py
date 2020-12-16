import pwn                  # $> pip install pwntools
pwn.context.arch = "amd64"  # docs.pwntools.com/en/stable/context.html

MAX_INSTRLEN = 8


def make_trans_struct(asm_instr):
    code = pwn.asm(asm_instr)
    code_str = "{" + ", ".join(
        str(byte) for byte in code
    ) + "}"

    if len(code) > MAX_INSTRLEN:
        raise ValueError("instruction too long")

    return "{%s, %d}," % (code_str, len(code))


trans_asm = {0: "nop",
             1: "xor eax,eax",
             2: "add eax,3",
             3: "dec eax",
             4: "mov eax,ebx",
             5: "nop",
             6: "add eax,6",    # superevents
             7: "add eax,9",
             8: "add eax,2",
             9: "mov eax,3",
             10: "mov eax,-1",
             11: "xor eax,eax\nxor ebx,ebx"}

pref = """
#define MAX_INSTRLEN %d                                                                 

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
""" % MAX_INSTRLEN

suf = "};"

transformations = "\n".join(
    make_trans_struct(asm_instr) for asm_instr in trans_asm.values()
)
transformations = pref + transformations + suf

prolog_asm = "push rbp\n mov rbp,rsp\n mov eax,edi\n mov ebx,esi"
prolog = "struct trans prolog = %s;\n" % make_trans_struct(prolog_asm)[:-1]

epilog_asm = "pop rbp\n ret"
epilog = "struct trans epilog = %s;\n" % make_trans_struct(epilog_asm)[:-1]

transformations = transformations + "\n" + prolog + epilog

with open("transformations.c", "w") as f:
    f.write(transformations)
