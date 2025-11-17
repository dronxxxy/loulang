add_rules("mode.debug", "mode.release")

target("logger")
    set_kind("shared")
    set_languages("c++20")
    add_includedirs("include", {
      public = true
    })
    add_includedirs("src")
    add_files("src/*.cpp")
