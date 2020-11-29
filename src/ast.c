#include "common.h"
#include "ast.h"
#include "token.h"
#include "alloc.h"

owo_tbase owo_tint[1] = { OWT_INT };
owo_tbase owo_tchar[1] = { OWT_CHAR };
owo_tbase owo_tbool[1] = { OWT_BOOL };
owo_tbase owo_tfloat[1] = { OWT_FLOAT };

// TODO: make this local
static thread_local mem_arena *cur_ar;

void ast_init(owo_ast *ast)
{
	ast->ctrs = 0;
	arena_init(&ast->ar, &system_allocator, 1024 * 1024);
	cur_ar = &ast->ar;
}

void ast_fini(owo_ast *ast)
{
	arena_fini(&ast->ar);
	cur_ar = NULL;
}

static void *ast_alloc(len_t sz)
{
	return arena_alloc(cur_ar, ALIGN_UP_P2(sz, 16));
}

owo_expr owe_int(uint64_t val)
{
	struct owo_eint *expr = ast_alloc(sizeof *expr);
	expr->base = OWE_INT;
	expr->val = val;
	return &expr->base;
}

owo_expr owe_ident(ident_t ident)
{
	struct owo_eident *expr = ast_alloc(sizeof *expr);
	expr->base = OWE_IDENT;
	expr->ident = ident;
	return &expr->base;
}


owo_construct owc_funcdef(allocator al, ident_t name, owo_type ret, small_buf params, small_buf body)
{
	struct owo_cfuncdef *ctr = ast_alloc(sizeof *ctr);
	ctr->base = OWC_FUNC;
	ctr->name = name;
	ctr->ret = ret;
	sm_shrink_into(al, &ctr->params, params, sizeof (struct owo_param));
	sm_shrink_into(al, &ctr->body, body, PTRSZ);
	return &ctr->base;
}

owo_stmt ows_sreturn(owo_expr rval)
{
	struct owo_sreturn *stmt = ast_alloc(sizeof *stmt);
	stmt->base = OWS_RETURN;
	stmt->rval = rval;
	return &stmt->base;
}

owo_stmt ows_var(ident_t name, owo_type type, owo_expr init)
{
	struct owo_svar *stmt = ast_alloc(sizeof *stmt);
	stmt->base = OWS_VAR;
	stmt->name = name;
	stmt->type = type;
	stmt->init = init;
	return &stmt->base;
}

owo_type owt_ptr(owo_type t)
{
	struct owo_tptr *type = ast_alloc(sizeof *type);
	type->base = OWT_PTR;
	type->inner = t;
	return &type->base;
}

