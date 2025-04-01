cmake_minimum_required(VERSION 3.10) # Or higher as needed

project(LabHash)

# Set include directories
include_directories(
    .
)

# Source files
set(SOURCE_FILES
    testHash.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Target-specific options (optional)
target_compile_options(${PROJECT_NAME} PRIVATE
    # Add any compiler flags here
)

# Link libraries (optional)
# target_link_libraries(${PROJECT_NAME}
#     # List libraries here
# )
