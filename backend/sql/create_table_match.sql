create table if not exists replays (
    id integer primary key autoincrement,
    name text,
    date text,
    duration text,
    data blob
);
