#ifndef list_h
#define list_h

#define IMPL_MAP(XX) \
	XX(p1a1)         \
	XX(minecraft)


#define XX(name) void impl_##name();
IMPL_MAP(XX)
#undef XX


typedef void(*impl_t)();

#define NAME_TO_IMPL_HELPER(name) { #name, impl_##name },
#define NAME_TO_IMPL { IMPL_MAP(NAME_TO_IMPL_HELPER) }

#endif // list_h
