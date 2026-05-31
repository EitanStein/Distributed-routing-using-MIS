include_guard(GLOBAL)

function(create_project_options)
    option(ENABLE_SANITIZERS "Enable Sanitizers in Debug mode" OFF)

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

    # ---------------------------------------------------------
    # WARNING FLAGS
    # ---------------------------------------------------------
    if(TRACY_ENABLE)
        target_compile_options(project_options INTERFACE
            # Windows Warnings: Level 4 and treat as errors
            $<$<AND:${IS_MSVC}>:/W4>
            
            # Linux Warnings: Wall, Wextra, Pedantic, and treat as errors
            $<$<AND:${IS_POSIX}>:-Wall -Wextra -Wpedantic>
        )
    else()
        target_compile_options(project_options INTERFACE
            # Windows Warnings: Level 4 and treat as errors
            $<$<AND:${IS_MSVC}>:/W4 /WX>
            
            # Linux Warnings: Wall, Wextra, Pedantic, and treat as errors
            $<$<AND:${IS_POSIX}>:-Wall -Wextra -Wpedantic -Werror>
        )
    endif()

    # ---------------------------------------------------------
    # SANITIZER FLAGS (Only if enabled and in Debug mode)
    # ---------------------------------------------------------
    if(ENABLE_SANITIZERS)
        
        # --- MSVC (Windows) Sanitizers ---
        target_compile_options(project_options INTERFACE
            $<$<AND:${IS_MSVC},${IS_DEBUG}>:/fsanitize=address>
        )
        target_link_options(project_options INTERFACE
            $<$<AND:${IS_MSVC},${IS_DEBUG}>:/fsanitize=address>
        )

        # --- GCC/Clang (Linux) Sanitizers ---
        target_compile_options(project_options INTERFACE
            $<$<AND:${IS_POSIX},${IS_DEBUG}>:-fsanitize=address,undefined -fno-omit-frame-pointer>
        )
        target_link_options(project_options INTERFACE
            $<$<AND:${IS_POSIX},${IS_DEBUG}>:-fsanitize=address,undefined>
        )
        
    endif()

endfunction()