Name: Bader Faris
Student Number: 101304229

information for each file:

README.txt: Explains everything 
Relational_Algabra: all the code written 
Makefile: used for running the code

Intructions For Mac(any other tech, unknown if it runs the same):
Download Zipped file
unzip file
drag into Vscode or VM of your choosing
open terminal 
using "ls" and "cd" navigate to unzipped folder(make sure you enter the Bonus file)
type "make clean"
then type "make"
lastly "make run"

Important information while running the code:
1. for the ⋃, ∩, −, the file columns must have the same names for it to work and in the same order
2. equations are read right to left, so for example #name=Alex Employee, it would read employee first
3. "Selection=#, Projection=$, Join=%, Union=|, intersection=&, and Difference=-\n"
4. "Examples:  #condition TableName   |   $col1,col2 TableName   |   Table1 % Table2
5. there is two ways to make data, one you input all information yourself, or two you can paste a whole block(this is very case sensative)
6. video will include a bunch of examples as well to showcase everything
7. Column name matching is case-insensitive during queries, but pasted table definitions are case sensitive.

Example, bunch of different styles with results(tested myself/also included in the video):

Employees (id, name, dept) = {
  1, Alex, Sales
  2, Bader, Finance
  3, Mo, HR
  4, Chris, Finance
}

Grads (id, name, dept) = {
  1, Alex, Sales
  5, Dana, Finance
  6, Lee, Marketing
  3, Mo, HR
}

Depts (dept, location) = {
  Sales, SF
  Finance, NY
  HR, LA
  Marketing, Boston
}
END

#dept=Finance Employees
2, Bader, Finance
4, Chris, Finance

$ name,dept Employees
Alex, Sales
Bader, Finance
Mo, HR
Chris, Finance

Employees % Depts
1, Alex, Sales, SF
2, Bader, Finance, NY
3, Mo, HR, LA
4, Chris, Finance, NY

$ name,dept Employees | $ name,dept Grads
Alex, Sales
Bader, Finance
Mo, HR
Chris, Finance
Dana, Finance
Lee, Marketing

$ name,dept Employees & $ name,dept Grads
Alex, Sales
Mo, HR

$ name,dept Employees - $ name,dept Grads
Bader, Finance
Chris, Finance

$ name ( #location=NY ( Employees % Depts ) )
Bader
Chris

$ name,dept ( ( #dept=Finance ( Employees | Grads ) ) % Depts )
Bader, Finance
Chris, Finance
Dana, Finance

#id>=3 Employees
3, Mo, HR
4, Chris, Finance

$ dept ( Employees % Depts )
Sales
Finance
HR
