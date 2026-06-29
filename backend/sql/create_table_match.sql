create table if not exists match (
    id text primary key,
    name text,
    date text,
    duration text,
    data blob
);
