syn keyword louKeywords public global extern
syn keyword louKeywords meta const final var
syn keyword louKeywords if else
syn keyword louKeywords return
syn keyword louKeywords fun

syn keyword louBuiltInTypes u8 u16 u32 u64
syn keyword louBuiltInTypes i8 i16 i32 i64
syn keyword louBuiltInTypes bool

syn keyword louBoolean true false

syn match louOneLineComment "#.*$"
syn region louMultiLineComment start='#`' end='`#' contains=louMultiLineComment 
syn match louIdent "[a-zA-Z_][a-zA-Z0-9_]*"
syn match louPlugin "@[a-zA-Z0-9_]*"
syn match louInteger "\d\+"
syn region louString start='c"' end='"'
syn region louString start='"' end='"'
syn region louChar start='\'' end='\''
syn match louEscapeChar "\\[nrt0]" containedIn=louString containedIn=louChar
syn match louEscapeChar "\\\"" containedIn=louString
syn match louEscapeChar "\\'" containedIn=louChar

hi def link louKeywords Keyword
hi def link louPlugin PreProc
hi def link louMultiLineComment Comment
hi def link louOneLineComment Comment
hi def link louString Constant
hi def link louChar Constant
hi def link louEscapeChar SpecialChar
hi def link louInteger Constant
hi def link louBoolean Constant
hi def link louBuiltInTypes Type

