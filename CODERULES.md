# General

This is code rules(for C++) for KoalaEngine. 


# Naming Conventions

- All class/struct/enum_type name **must** use UpperCamelCase naming.
- All enum_type name **must** start with 'E'. The members in enum **must** UpperCamelCase and not start with enum_type name. 

For example: `enum EMyEnum {Something;};` is a OK naming, `enum MyEnum {MyEnumSomething;};` is a not OK naming.

- All constants **must** use UpperCamelCase naming. 
- All function name **must** use UpperCamelCase naming.
- All data member of class/struct, local variable **must** use lowerCamelCase naming. 
- Global variables **must** naming must start with 'G' and use UpperCamelCase. like 'GMyGlobalVariable'.
- Macros, use ALL_UPPER naming.

# Namespaces

- All Engine, Editor codes must within `Koala` namespace. but you **can** define sub-namespace within this namespace.