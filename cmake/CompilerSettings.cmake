include_guard(GLOBAL)

function(create_project_options)
    option(ENABLE_SANITIZERS "Enable Sanitizers in Debug mode" OFF)
    option(ASAN_UBSAN "Enable Address and Undefined behaviour sanitizers" OFF)
    option(TSAN "Enable Thread sanitizer" OFF)
    option(MSAN "Enable Memory sanitizer" OFF)

    option(TRACY_ENABLE "Enable Tracy profiling" OFF)
    option(TRACY_BUILD_PROFILER "Build Tracy GUI Profiler App automatically" OFF)

    option(BUILD_BENCHMARKS "Build the project benchmarks" OFF)

    if(NOT TARGET project_options)
        add_library(project_options INTERFACE)
    endif()

    set(IS_MSVC  $<CXX_COMPILER_ID:MSVC>)
    set(IS_GCC   $<CXX_COMPILER_ID:GNU>)
    set(IS_CLANG $<CXX_COMPILER_ID:Clang>)
    set(IS_DEBUG $<CONFIG:Debug>)
    
    set(IS_POSIX $<OR:${IS_GCC},${IS_CLANG}>)


    if(TRACY_ENABLE)
        target_compile_options(project_options INTERFACE
            $<$<AND:${IS_MSVC}>:/W4>
            
            $<$<AND:${IS_POSIX}>:-Wall -Wextra -Wpedantic>
        )
    else()
        target_compile_options(project_options INTERFACE
            $<$<AND:${IS_MSVC}>:/W4 /WX>
            
            $<$<AND:${IS_POSIX}>:-Wall -Wextra -Wpedantic -Werror>
        )
    endif()


    if(ENABLE_SANITIZERS)
        if(ASAN_UBSAN)
            target_compile_options(project_options INTERFACE
                $<$<AND:${IS_MSVC},${IS_DEBUG}>:/fsanitize=address>
            )
            target_link_options(project_options INTERFACE
                $<$<AND:${IS_MSVC},${IS_DEBUG}>:/fsanitize=address>
            )
        

            target_compile_options(project_options INTERFACE
                $<$<AND:${IS_POSIX},${IS_DEBUG}>:-fsanitize=address,undefined -fno-omit-frame-pointer>
            )
            target_link_options(project_options INTERFACE
                $<$<AND:${IS_POSIX},${IS_DEBUG}>:-fsanitize=address,undefined>
            )
        elseif(TSAN)
            target_compile_options(project_options INTERFACE
                $<$<AND:${IS_POSIX},${IS_DEBUG}>:-fsanitize=thread>
            )
            target_link_options(project_options INTERFACE
                $<$<AND:${IS_POSIX},${IS_DEBUG}>:-fsanitize=thread>
            )
        elseif(MSAN)
            target_compile_options(project_options INTERFACE
                $<$<AND:${IS_CLANG},${IS_DEBUG}>:-fsanitize=memory -fsanitize-memory-track-origins=2>
            )
            target_link_options(project_options INTERFACE
                $<$<AND:${IS_CLANG},${IS_DEBUG}>:-fsanitize=memory>
            )
        endif()
        
    endif()

endfunction()