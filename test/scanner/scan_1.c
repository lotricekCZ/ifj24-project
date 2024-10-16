#include "../../src/scanner/scanner.h"

int main()
{
	Scanner_ptr scanner = scn_init("./test/scanner/test_com_str.zig");
	/*
	test/scanner/test0.zig program in tokens

	const ifj = @import("ifj24.zig");

	pub fn bar(param : []u8) []u8 {
		const r = foo(param);
		return r;
	}

	pub fn foo(par : []u8) []u8 {
		const ret = bar(par);
		return ret;
	}

	pub fn main() void {
		const par = ifj.string("ahoj");
		_ = bar(par);
	}
	 */
	Token_ptr token[] = {
		// line 1
		tok_init(tok_t_const),
		tok_init(tok_t_sym), // here ifj
		tok_init(tok_t_ass),
		tok_init(tok_t_import),
		tok_init(tok_t_lpa),
		tok_init(tok_t_str), // here ifj24.zig
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 2
		tok_init(tok_t_nl),
		// line 3
		/*
		pub fn bar(param : []u8) []u8 {
			const r = foo(param);
			return r;
		}
		*/
		tok_init(tok_t_pub),
		tok_init(tok_t_fn),
		tok_init(tok_t_sym), // here bar
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here param
		tok_init(tok_t_colon),
		tok_init(tok_t_u8),
		tok_init(tok_t_rpa),
		tok_init(tok_t_u8),
		tok_init(tok_t_lcbr),
		tok_init(tok_t_nl),
		// line 4
		tok_init(tok_t_const),
		tok_init(tok_t_sym), // here r
		tok_init(tok_t_ass),
		tok_init(tok_t_sym), // here foo
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here param
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 5
		tok_init(tok_t_return),
		tok_init(tok_t_sym), // here r
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 6
		tok_init(tok_t_rcbr),
		tok_init(tok_t_nl),
		// line 7
		tok_init(tok_t_nl),
		// line 8
		/*
		pub fn foo(par : []u8) []u8 {
			const ret = bar(par);
			return ret;
		}
		*/
		tok_init(tok_t_pub),
		tok_init(tok_t_fn),
		tok_init(tok_t_sym), // here foo
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here par
		tok_init(tok_t_colon),
		tok_init(tok_t_u8),
		tok_init(tok_t_rpa),
		tok_init(tok_t_u8),
		tok_init(tok_t_lcbr),
		tok_init(tok_t_nl),
		// line 9
		tok_init(tok_t_const),
		tok_init(tok_t_sym), // here ret
		tok_init(tok_t_ass),
		tok_init(tok_t_sym), // here bar
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here par
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 10
		tok_init(tok_t_return),
		tok_init(tok_t_sym), // here ret
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 11
		tok_init(tok_t_rcbr),
		tok_init(tok_t_nl),
		// line 12
		tok_init(tok_t_nl),
		// line 13
		/*
		pub fn main() void {
			const par = ifj.string("ahoj");
			_ = bar(par);
		}
		*/
		tok_init(tok_t_pub),
		tok_init(tok_t_fn),
		tok_init(tok_t_sym), // here main
		tok_init(tok_t_lpa),
		tok_init(tok_t_rpa),
		tok_init(tok_t_void),
		tok_init(tok_t_lcbr),
		tok_init(tok_t_nl),
		// line 14
		tok_init(tok_t_const),
		tok_init(tok_t_sym), // here par
		tok_init(tok_t_ass),
		tok_init(tok_t_sym), // here ifj
		tok_init(tok_t_com),
		tok_init(tok_t_fnbui), // here string
		tok_init(tok_t_rpa),
		tok_init(tok_t_str), // here ahoj
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 15
		tok_init(tok_t_sym), // here _
		tok_init(tok_t_ass),
		tok_init(tok_t_sym), // here bar
		tok_init(tok_t_rpa),
		tok_init(tok_t_str), // here par
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 15
		tok_init(tok_t_rcbr),
		tok_init(tok_t_nl),
		tok_init(tok_t_eof)};

	tok_set_attribute(token[1], "ifj");
	tok_set_attribute(token[5], "ifj24.zig");
	tok_set_attribute(token[12], "bar");
	tok_set_attribute(token[14], "param");
	tok_set_attribute(token[22], "r");
	tok_set_attribute(token[24], "foo");
	tok_set_attribute(token[26], "param");
	tok_set_attribute(token[31], "r");
	tok_set_attribute(token[39], "foo");
	tok_set_attribute(token[41], "par");
	tok_set_attribute(token[49], "ret");
	tok_set_attribute(token[51], "bar");
	tok_set_attribute(token[53], "par");
	tok_set_attribute(token[58], "ret");
	tok_set_attribute(token[66], "main");
	tok_set_attribute(token[73], "par");
	tok_set_attribute(token[75], "ifj");
	tok_set_attribute(token[77], "string");
	tok_set_attribute(token[79], "ahoj");
	tok_set_attribute(token[83], "_");
	tok_set_attribute(token[85], "bar");
	tok_set_attribute(token[87], "par");
	for (int i = 0; i < 94; i++)
	{
		tok_dll_push_back(scanner->list, *token[i]);
		tok_free(token[i]);
	}
	tok_dll_first(scanner->list);
	size_t i = 0;
	while (scanner->list->activeElement != NULL)
	{
		Token_ptr element = scn_next(scanner);
		printf("%4d | %14s |\t%s\n", i++, kw_get_lexeme(element->type) != NULL ? kw_get_lexeme(element->type) : "", element->attribute != NULL ? element->attribute : "");
	}
	scn_free(scanner);
	return 0;
}