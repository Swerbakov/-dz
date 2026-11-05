-- Домашнее задание «Работа с SQL. Создание БД»
-- Задание 1: схема музыкального сервиса

-- Жанры
CREATE TABLE IF NOT EXISTS genre (
    id   SERIAL       PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE
);

-- Исполнители
CREATE TABLE IF NOT EXISTS artist (
    id   SERIAL       PRIMARY KEY,
    name VARCHAR(100) NOT NULL
);

-- Связь «исполнитель — жанр» (многие ко многим)
CREATE TABLE IF NOT EXISTS artist_genre (
    artist_id INTEGER NOT NULL REFERENCES artist(id) ON DELETE CASCADE,
    genre_id  INTEGER NOT NULL REFERENCES genre(id)  ON DELETE CASCADE,
    PRIMARY KEY (artist_id, genre_id)
);

-- Альбомы
CREATE TABLE IF NOT EXISTS album (
    id           SERIAL       PRIMARY KEY,
    title        VARCHAR(150) NOT NULL,
    release_year INTEGER      NOT NULL CHECK (release_year > 0)
);

-- Связь «исполнитель — альбом» (многие ко многим)
CREATE TABLE IF NOT EXISTS artist_album (
    artist_id INTEGER NOT NULL REFERENCES artist(id) ON DELETE CASCADE,
    album_id  INTEGER NOT NULL REFERENCES album(id)  ON DELETE CASCADE,
    PRIMARY KEY (artist_id, album_id)
);

-- Треки (каждый трек принадлежит ровно одному альбому)
CREATE TABLE IF NOT EXISTS track (
    id       SERIAL       PRIMARY KEY,
    title    VARCHAR(150) NOT NULL,
    duration INTEGER      NOT NULL CHECK (duration > 0), -- в секундах
    album_id INTEGER      NOT NULL REFERENCES album(id) ON DELETE CASCADE
);

-- Сборники
CREATE TABLE IF NOT EXISTS collection (
    id           SERIAL       PRIMARY KEY,
    title        VARCHAR(150) NOT NULL,
    release_year INTEGER      NOT NULL CHECK (release_year > 0)
);

-- Связь «сборник — трек» (многие ко многим)
CREATE TABLE IF NOT EXISTS collection_track (
    collection_id INTEGER NOT NULL REFERENCES collection(id) ON DELETE CASCADE,
    track_id      INTEGER NOT NULL REFERENCES track(id)      ON DELETE CASCADE,
    PRIMARY KEY (collection_id, track_id)
);
