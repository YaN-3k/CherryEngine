function(enable_warnings target_name)
  if(CHERRY_ENGINE_BUILD_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
      list(APPEND msvc_options "/W3")
      if(MSVC_VERSION GREATER 1900) # Allow non fatal security warnings for msvc 2015
        list(APPEND msvc_options "/WX")
      endif()
    endif()

    target_compile_options(
      ${target_name}
      PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
      -Wall
      -Wextra
      -Wconversion
      -pedantic
      -Wfatal-errors>
      $<$<CXX_COMPILER_ID:MSVC>:${msvc_options}>)
  endif()
endfunction()
