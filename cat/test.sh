#!/ bin / bash

success_count=0
fail_count=0
file="test.txt"
flags=(-b -e -n -s -t -v)

for flag1 in "${flags[@]}"
do
    all_flags="$flag1 $file $file"
    echo "$all_flags"
    
    ./s21_cat $all_flags > s21_cat_output.txt
    cat $all_flags > cat_output.txt

    if cmp -s "s21_cat_output.txt" "cat_output.txt"
    then
        (( success_count++ ))
    else
        (( fail_count++ ))
    fi

    rm s21_cat_output.txt cat_output.txt
done

echo "success: $success_count"
echo "fail: $fail_count"
