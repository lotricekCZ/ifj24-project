/**
 * @addtogroup IFJ2024
 * @file scanner.h
 * @brief Knihovna pro lexikalni analyzu
 * @author xramas01; Jakub Ramaseuski
 *
 * Funkce scn jsou pro externi uziti a primo pracuji se skenerem
 * funkce a makra s prefixem sca jsou POUZE pro interni uziti, s pomoci
 * nichz se primo ovlivnuje graf skeneru, cesty v nem a podminky.
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include "../utils/token.h"
#include "../utils/token_dll.h"
#include "../utils/token_types.h"
#include "scan_state.h"

/**
 * @brief Podmínka pro řetězce. Kontroluje, zda znak c je tisknutelný a není zpětným lomítkem či uvozovkou.
 *
 * @param c Znak, který má být zkontrolován.
 * @return Vrací nenulovou hodnotu, pokud je znak tisknutelný a není zpětným lomítkem či uvozovkou, jinak vrací 0.
 */
int sca_string(int c);

/**
 * @def SCA_MATCH_DECL(name, a)
 * @brief Deklarace funkce pro kontrolu shody znaku
 * @param name Jmeno funkce
 * @param a Hodnota pro kontrolu
 *
 * Deklaruje funkci pro kontrolu shody znaku.
 */
#define SCA_MATCH_DECL(name, a) \
    int m_chk_##name(int b) { return b == (a); }

/**
 * @def SCA_MATCH(name)
 * @brief Vyvolani funkce pro kontrolu shody znaku
 * @param name Jmeno funkce
 *
 * Vyvolava funkci pro kontrolu shody znaku.
 */
#define SCA_MATCH(name) m_chk_##name

/**
 * @def SCA_GREATER_DECL(name, a)
 * @brief Deklarace funkce pro kontrolu vetsi hodnoty
 * @param name Jmeno funkce
 * @param a Hodnota pro kontrolu
 *
 * Deklaruje funkci pro kontrolu vetsi hodnoty.
 */
#define SCA_GREATER_DECL(name, a) \
    int m_chk_##name(int b) { return b > (a); }

/**
 * @def SCA_GREATER(name)
 * @brief Vyvolani funkce pro kontrolu vetsi hodnoty
 * @param name Jmeno funkce
 *
 * Vyvolava funkci pro kontrolu vetsi hodnoty.
 */
#define SCA_GREATER(name) m_chk_##name

/**
 * @def SCA_PATH_DECL(src, dest)
 * @brief Deklarace struktury pro reprezentaci cesty v grafu skeneru
 * @param src Zdrojovy uzel
 * @param dest Destinacni uzel
 *
 * Deklaruje strukturu pro reprezentaci cesty v grafu skeneru.
 */
#define SCA_PATH_DECL(src, dest) Scan_path src##_to_##dest;

/**
 * @def SCA_PATH_DEF(src, dest, args...)
 * @brief Definice struktury pro reprezentaci cesty v grafu skeneru
 * @param src Zdrojovy uzel
 * @param dest Destinacni uzel
 * @param args... Pole funkci pro kontrolu shody znaku
 *
 * Definuje strukturu pro reprezentaci cesty v grafu skeneru.
 */
#define SCA_PATH_DEF(src, dest, args...) Scan_path src##_to_##dest = {.from = &src, .to = &dest, .matches = NULL, .count = 0};

/**
 * @def SCA_PATH_INIT(name, args...)
 * @brief Inicializace struktury pro reprezentaci cesty v grafu skeneru
 * @param name Jmeno struktury
 * @param args... Pole funkci pro kontrolu shody znaku
 *
 * Inicializuje strukturu pro reprezentaci cesty v grafu skeneru.
 */
#define SCA_PATH_INIT(name, args...)                                              \
    name.matches = imalloc(sizeof((int (*[])(int)){args}));                       \
    memcpy(name.matches, (int (*[])(int)){args}, sizeof((int (*[])(int)){args})); \
    name.count = sizeof((int (*[])(int)){args}) / sizeof(int (*)(int));

/**
 * @def SCA_PATH_DEINIT(name)
 * @brief Deinicializace struktury pro reprezentaci cesty v grafu skeneru
 * @param name Jmeno struktury
 *
 * Deinicializuje strukturu pro reprezentaci cesty v grafu skeneru.
 */
#define SCA_PATH_DEINIT(name) \
    ifree(name.matches);      \
    name.matches = NULL;

/**
 * @def SCA_PATH(src, dest)
 * @brief Ziskani struktury pro reprezentaci cesty v grafu skeneru
 * @param src Zdrojovy uzel
 * @param dest Destinacni uzel
 *
 * Vraci strukturu pro reprezentaci cesty v grafu skeneru.
 */
#define SCA_PATH(src, dest) src##_to_##dest

/**
 * @struct _Scanner
 * @brief Struktura reprezentujici scanner
 *
 * Struktura reprezentuje scanner, ktery se pouziva pro lexikalni
 * analyzu. Struktura obsahuje ukazatel na jmeno souboru, ktery
 * se ma analyzovat a ukazatel na dynamicke pole, kam se budou
 * ukladat tokeny, dále aktuální řádku a index v zdrojovém textu.
 */
typedef struct _Scanner
{
    char *file_name;
    tok_dllist *list;
    char *source;
    size_t source_index;
    size_t source_line;
    size_t source_size;
    size_t line;
    bool is_scanned;
} Scanner;

typedef struct _Scanner *Scanner_ptr;
typedef struct Scan_node Scan_node;
typedef struct Scan_path Scan_path;

/**
 * @struct Scan_node
 * @brief Uzel grafu skeneru
 *
 * Struktura reprezentuje jeden uzel v grafu skeneru. Uzel
 * obsahuje pole ukazatelu na dalsi uzly, ktere jsou
 * dostupne z tohoto uzlu.
 */
struct Scan_node
{
    // podmínky, které se kontrolují, aby se urcilo, zda je dany znak shodny s uzlem
    Scan_path **children;
    size_t count;
    scn_state_t state;
};
typedef Scan_node *Scan_node_ptr;

struct Scan_path
{
    int (**matches)(int);
    size_t count;
    Scan_node *from;
    Scan_node *to;
};

/**
 * @brief Vytvori scanner z konfiguracního souboru.
 * @details Vytvori strukturu Scanner a inicializuje ji s obsahem souboru filename.
 * @param filename Cesta k souboru, ze ktereho chceme cist.
 * @return Struktura Scanner, nebo NULL, pokud doslo k chybe.
 */
Scanner_ptr scn_init(char *filename);
/**
 * @brief Uvolní paměť, která byla alokována pro scanner.
 *
 * Tato funkce uvolní paměť, která byla alokována pro scanner a jeho vnitřní
 * proměnné. Protože scanner používá dynamicky alokovanou paměť, je nutné
 * tuto funkci volat před koncem programu, aby se zabránilo úniku paměti.
 *
 * @param scanner Ukazatel na scanner, jehož paměť má být uvolněna.
 */
void scn_free(Scanner_ptr scanner);

/**
 * @brief Vrati token, ktery je na danem indexu v zdrojovem textu.
 * @details Funkce projde grafem skeneru a nalezne token, ktery je
 * na danem indexu v zdrojovem textu. Pokud je nalezen token, vraci
 * se ukazatel na strukturu, ktera reprezentuje tento token.
 * Pokud token neexistuje, vraci se NULL.
 * @param scanner Struktura, ktera reprezentuje scanner.
 * @returns Ukazatel na strukturu, ktera reprezentuje token, nebo NULL, pokud token neexistuje.
 */
Token_ptr scn_scan(Scanner_ptr scanner);

/**
 * @brief Ziskani dalsiho tokenu
 *
 * Funkce vraci dalsi token v seznamu a posune aktivni prvek na dalsi.
 *
 * @param scanner Ukazatel na strukturu reprezentujici scanner
 * @return Ukazatel na strukturu reprezentujici token
 */
Token_ptr scn_next(Scanner_ptr scanner);

/**
 * @brief Otevreni souboru se zdrojovym kodem
 *
 * Funkce otevira soubor se zdrojovym kodem, nacte jeho obsah do pameti
 * a vrati ukazatel na dynamicke pole, kde je ulozen zdrojovy kod.
 *
 * @return Ukazatel na dynamicke pole, kde je ulozen zdrojovy kod
 */
char *scn_open_file(Scanner_ptr scanner);

/**
 * @brief Kompozice chyboveho hlaseni.
 *
 * Funkce vraci retezec, ktery popisuje lexikalni chybu.
 * Retezec ma format "Lexical error on line <cislo_radky>: <radek_zdrojoveho_kоdу>\n<^>".
 *
 * @param scanner Struktura, ktera reprezentuje scanner.
 * @returns string, ktery popisuje lexikalni chybu.
 */
char *scn_compose_message(Scanner_ptr scanner);

// funkce pro analyzu skeneru (SCA), pouziti omezeno na soubor scanner.c

/**
 * @brief dosadi cesty k Scan_node.
 * @details Tato funkce pouziva seznam variabilnich argumentu k pripojeni synu
 * ke Scan_node.
 * @param node Scan_node, ke ktere mame pripojit deti.
 * @param argc Pocet argumentu.
 * @param ...
 */
void sca_assign_children(Scan_node_ptr node, int argc, ...);

/**
 * @brief Uvolni pamet, kterou mel uzel skeneru.
 * @details Funkce projde vsechny cesty, ktere z uzlu vedou, uvolni
 * pamet, kterou mely. Pak uvolni pamet, kterou mel na cesty
 *
 * @param node Uzel, jehoz cesty se maji uvolnit.
 */
void sca_free(Scan_node_ptr node);

/**
 * @brief Vyhleda v poli podminek, zdali je splnena nejaka podminka.
 * @details Vyhleda v poli podminek, zdali je splnena nejaka podminka.
 * Podminky jsou v OR vztahu, takze jakmile najde splnenou podminku, funkce
 * skonci a vraci true. Pokud nesplni zadnou podminku, vraci false.
 * @param path Ukazatel na strukturu Scan_path, ktera obsahuje pole podminek.
 * @param c Char, ktery se ma zkontrolovat.
 * @returns true, pokud byla splnena nejaka podminka, jinak false.
 */
bool sca_p_has_match(Scan_path *path, char c);

/**
 * @brief Najde cestu z uzlu, ktera vyhovuje danemu znaku.
 * @details Iterativne projde cesty a hleda takovou, ktera vyhovuje danemu znaku.
 * Pokud takova cesta je nalezena, vraci ji. Pokud ne, vraci NULL.
 * @param node Uzel, ktery se ma prohledat.
 * @param c Znak, ktery se ma hledat.
 * @returns Cesta, ktera vyhovuje danemu znaku, nebo NULL, pokud takova cesta neexistuje.
 */
Scan_path *sca_n_has_match(Scan_node *node, char c);

/**
 * @brief Zpracuje viceradkovy retezec.
 * 
 * @param scanner Ukazatel na strukturu Scanner.
 * @param index Index, na kterem zacina viceradkovy retezec.
 * @returns Viceradkovy retezec.
 */
char *scn_parse_multiline(Scanner_ptr scanner, size_t index);

#endif /* SCANNER_H */

/*** Konec souboru scanner.h ***/