FUNC_SUCCESS=0
FUNC_FAIL=0
DIFF=""

e="-e str"
f="-f file3.txt"

declare -a flags=(
"-i"
"-v"
"-c"
"-l"
"-n"
"-o"
"-h"
"-s"
)

declare -a files=(
"file1.txt file2.txt"
)

declare -a commline=(
"OPT FILE"
)

function testing() {
    str=$(echo "$@" | sed "s/OPT/$options/")
    str=$(echo "$str" | sed "s/FILE/$file/g")
    ./s21_grep $str > s21_grep_testing.log
    grep $str > system_grep_testing.log
    ((COUNTER++))
    if cmp -s "s21_grep_testing.log" "system_grep_testing.log"; then
        ((FUNC_SUCCESS++))
        echo "grep $str success"
    else
        ((FUNC_FAIL++))
        echo "grep $str fail"
    fi
    rm s21_grep_testing.log system_grep_testing.log
}

for file in "${files[@]}"; do
    options="$e"
    testing "${commline[@]}"

    for opt1 in "${flags[@]}"; do
        options="$e $opt1"
        testing "${commline[@]}"
    done

    for opt1 in "${flags[@]}"; do
        for opt2 in "${flags[@]}"; do
            if [ "$opt1" != "$opt2" ]; then
                if ! { [ "$opt1" == "-v" ] && [ "$opt2" == "-o" ]; } && ! { [ "$opt1" == "-o" ] && [ "$opt2" == "-v" ]; }; then
                    options="$e $opt1 $opt2"
                    testing "${commline[@]}"
                fi
            fi
        done
    done
done

file="file2.txt"
options="$f"
testing "${commline[@]}"

for opt1 in "${flags[@]}"; do
    if ! { [ "$opt1" == "-c" ] || [ "$opt1" == "-o" ]; }; then
        options="$f $opt1"
        testing "${commline[@]}"
    fi
done

for file in "${files[@]}"; do
    options="str"
    testing "${commline[@]}"
    for opt1 in "${flags[@]}"; do
        options="$opt1 str"
        testing "${commline[@]}"
    done

    for opt1 in "${flags[@]}"; do
        for opt2 in "${flags[@]}"; do
            if [ "$opt1" != "$opt2" ]; then
                if ! { [ "$opt1" == "-v" ] && [ "$opt2" == "-o" ]; } && ! { [ "$opt1" == "-o" ] && [ "$opt2" == "-v" ]; }; then
                    options="$opt1 $opt2 str"
                    testing "${commline[@]}"
                fi
            fi
        done
    done
done

echo "fail: $FUNC_FAIL"
echo "success: $FUNC_SUCCESS"
