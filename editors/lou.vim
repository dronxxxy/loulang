syn keyword louKeywords public global extern
syn keyword louKeywords meta const final var
syn keyword louKeywords return
syn keyword louKeywords fun

syn keyword louBuiltInTypes u8 u16 u32 u64
syn keyword louBuiltInTypes i8 i16 i32 i64
syn keyword louBuiltInTypes bool

syn match louOneLineComment "#.*$"
syn region louMultiLineComment start='#`' end='`#' contains=louMultiLineComment 
syn match louIdent "[a-zA-Z_$][a-zA-Z0-9_$]*"
syn match louInteger "\d\+"

hi def link louKeywords Keyword
hi def link louMultiLineComment Comment
hi def link louOneLineComment Comment
hi def link louInteger Constant
hi def link louBuiltInTypes Type

