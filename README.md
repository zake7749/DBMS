# Database management system

This is an exercise on implementing a small database management system which has an hashtable based on hash33.

## Usage
```
SELECT (DISTINCT) {attrs} FROM {tables} WHERE {conditions}
```

### Example
```SQL
SELECT * FROM books;
SELECT DISTINCT author FROM books;
SELECT title FROM books,sellRecord WHERE isbn=isbn_no AND author='J. K. Rowling';
```

## Data format

```
for sellRecord.txt
/* uid|no|isbn_no */
1|1|439139597
```

```
for books.txt
/* isbn|author|title|price|subject */
439139597|J. K. Rowling|Harry Potter and the goblet of fire|25.95|CHILDREN
```