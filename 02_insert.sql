-- Задание 1: заполнение базы данных

-- Жанры (4)
INSERT INTO genre (id, name) VALUES
    (1, 'Рок'),
    (2, 'Поп'),
    (3, 'Хип-хоп'),
    (4, 'Электроника');

-- Исполнители (6)
INSERT INTO artist (id, name) VALUES
    (1, 'Луна'),
    (2, 'Орион'),
    (3, 'Тихий Дом'),
    (4, 'Alex Stone'),
    (5, 'Vega'),
    (6, 'Мария Лето');

-- Исполнители и жанры
INSERT INTO artist_genre (artist_id, genre_id) VALUES
    (1, 1), (1, 2),   -- Луна: рок, поп
    (2, 4),           -- Орион: электроника
    (3, 1),           -- Тихий Дом: рок
    (4, 3), (4, 2),   -- Alex Stone: хип-хоп, поп
    (5, 4), (5, 3),   -- Vega: электроника, хип-хоп
    (6, 2);           -- Мария Лето: поп

-- Альбомы (6)
INSERT INTO album (id, title, release_year) VALUES
    (1, 'Первый свет',     2018),
    (2, 'Midnight Roads',  2019),
    (3, 'Город без огней', 2020),
    (4, 'Stone Age',       2020),
    (5, 'Летний дождь',    2021),
    (6, 'Echoes',          2019);

-- Исполнители и альбомы
INSERT INTO artist_album (artist_id, album_id) VALUES
    (1, 1),           -- Первый свет — Луна
    (2, 2), (5, 2),   -- Midnight Roads — Орион и Vega
    (3, 3), (1, 3),   -- Город без огней — Тихий Дом и Луна
    (4, 4),           -- Stone Age — Alex Stone
    (6, 5),           -- Летний дождь — Мария Лето
    (5, 6), (4, 6);   -- Echoes — Vega и Alex Stone

-- Треки (14), длительность в секундах
INSERT INTO track (id, title, duration, album_id) VALUES
    (1,  'Первый свет',     215, 1),
    (2,  'Мой путь',        245, 1),
    (3,  'Тишина',          198, 1),
    (4,  'Midnight Roads',  312, 2),
    (5,  'My Signal',       224, 2),
    (6,  'Neon',            187, 2),
    (7,  'Город без огней', 268, 3),
    (8,  'Мой город',       210, 3),
    (9,  'Дорога домой',    232, 3),
    (10, 'Stone Cold',      205, 4),
    (11, 'My Way Out',      176, 4),
    (12, 'Летний дождь',    189, 5),
    (13, 'Echo',            105, 6),
    (14, 'Pulse',           260, 6);

-- Сборники (5)
INSERT INTO collection (id, title, release_year) VALUES
    (1, 'Лучшее за год',    2018),
    (2, 'Night Drive',      2019),
    (3, 'Хиты десятилетия', 2020),
    (4, 'Summer Mix',       2021),
    (5, 'Тихие вечера',     2019);

-- Сборники и треки (треки 9 и 14 намеренно не входят ни в один сборник)
INSERT INTO collection_track (collection_id, track_id) VALUES
    (1, 1), (1, 2), (1, 4),
    (2, 4), (2, 5), (2, 6), (2, 13),
    (3, 2), (3, 7), (3, 8), (3, 10),
    (4, 5), (4, 11), (4, 12),
    (5, 1), (5, 3), (5, 12);

-- Синхронизация счётчиков SERIAL после вставки с явными id
SELECT setval('genre_id_seq',      (SELECT MAX(id) FROM genre));
SELECT setval('artist_id_seq',     (SELECT MAX(id) FROM artist));
SELECT setval('album_id_seq',      (SELECT MAX(id) FROM album));
SELECT setval('track_id_seq',      (SELECT MAX(id) FROM track));
SELECT setval('collection_id_seq', (SELECT MAX(id) FROM collection));
