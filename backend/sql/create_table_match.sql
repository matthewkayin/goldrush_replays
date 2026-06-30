create table if not exists match (
    id integer primary key autoincrement,
    name text,
    date text,
    duration text,
    data blob
);
