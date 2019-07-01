
i=1
shouldExit=0
printf "\n"
while [ $i -lt $2 ]; 
do
	printf "Test "
	printf $i
	printf ": Exited with "
	./$1 <input$i
	ex=$?
	echo $ex
	echo "Should have exited with $shouldExit"
	if [ $ex -eq $shouldExit ] 
	then
		printf "Success!\n\n"
	else
		printf "Fail!\n\n"
	fi
	let i=i+1
done