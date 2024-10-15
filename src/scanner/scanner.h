/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include "../utils/token.h"
#include "../utils/token_dll.h"
#include "../utils/keywords.h"
#include "scan_state.h"

#define SCA_MATCH_DECL(name, a) \
	int m_chk_##name(int b) { return b == (a); }
#define SCA_MATCH(name) m_chk_##name

#define SCA_GREATER_DECL(name, a) \
	int m_chk_##name(int b) { return b > (a); }
#define SCA_GREATER(name) m_chk_##name

#define SCA_PATH_DECL(src, dest) Scan_path src##_to_##dest;
#define SCA_PATH_DEF(src, dest, args...) Scan_path src##_to_##dest = {.from = &src, .to = &dest, .matches = NULL, .count = 0};
#define SCA_PATH_INIT(name, args...)                    \
	name.matches = malloc(sizeof((int *(*)(int)){args}));      \
	memcpy(name.matches, (int (*[])(int)){args}, sizeof((int (*[])(int)){args})); \
	name.count = sizeof((int (*[])(int)){args}) / sizeof(int (*)(int));
#define SCA_PATH_DEINIT(name) \
	free(name.matches);       \
	name.matches = NULL;
#define SCA_PATH(src, dest) src##_to_##dest

typedef struct _Scanner
{
	char *file_name;
	tok_dllist *list;
} Scanner;

typedef struct _Scanner *Scanner_ptr;

Scanner_ptr scn_init(char *filename);
void scn_free(Scanner_ptr scanner);
bool scn_scan(Scanner_ptr scanner);

Token_ptr scn_get_token(Scanner_ptr scanner);
Token_ptr scn_previous(Scanner_ptr scanner);
Token_ptr scn_next(Scanner_ptr scanner);
char *scn_open_file(Scanner_ptr scanner);

// scanner analyze functions, usage restricted to scanner.c
/// entry function
// Token_ptr sca_init();

typedef struct Scan_node Scan_node;
typedef struct Scan_path Scan_path;
/// recursive function tree scanner
struct Scan_node
{
	// conditions that are checked in order to determine given character matches the node
	Scan_path **children;
	size_t count;
};

struct Scan_path
{
	int (**matches)(int);
	size_t count;
	Scan_node *from;
	Scan_node *to;
};

typedef Scan_node *Scan_node_ptr;
// extern Scan_path sca_paths[];

Scan_node sca_init;

/// init accessible
extern Scan_node sca_underscore;
extern Scan_node sca_alpha;

extern Scan_node sca_qm;
extern Scan_node sca_bro;
extern Scan_node sca_brc;

extern Scan_node sca_at;
extern Scan_node sca_atid;

extern Scan_node sca_dt;
extern Scan_node sca_dtus;
extern Scan_node sca_fn;

extern Scan_node sca_int;
extern Scan_node sca_int2;
extern Scan_node sca_intdt;
extern Scan_node sca_intexp;
extern Scan_node sca_dec;
extern Scan_node sca_dec2;

Scan_node sca_s1;
extern Scan_node sca_s2;
extern Scan_node sca_s3;
extern Scan_node sca_s3;
extern Scan_node sca_s_max1;
extern Scan_node sca_s_max2;
Scan_node sca_s5;
Scan_node sca_str;

extern Scan_node sca_backslash;
extern Scan_node sca_ml1;
extern Scan_node sca_ml2;
extern Scan_node sca_ml3;
extern Scan_node sca_ml_max1;
extern Scan_node sca_ml_max2;
extern Scan_node sca_ml5;
extern Scan_node sca_ml6;
extern Scan_node sca_ml_str;

extern Scan_node sca_eof;
extern Scan_node sca_curlybrace_close;
extern Scan_node sca_curlybrace_open;
extern Scan_node sca_parenthese_open;
extern Scan_node sca_parenthese_close;
extern Scan_node sca_semicolon;
extern Scan_node sca_slash;
extern Scan_node sca_vertical;
extern Scan_node sca_colon;
extern Scan_node sca_exclamation;
extern Scan_node sca_greater;
extern Scan_node sca_equal;
extern Scan_node sca_less;
extern Scan_node sca_asterisk;
extern Scan_node sca_minus;
extern Scan_node sca_plus;
extern Scan_node sca_hashtag;

void sca_assign_children(Scan_node_ptr node, ...);

#endif