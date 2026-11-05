-- ===================== Задание 2 =====================

-- 1. Название и продолжительность самого длительного трека
SELECT title, duration
FROM track
WHERE duration = (SELECT MAX(duration) FROM track);

-- 2. Названия треков, продолжительность которых не менее 3,5 минут (210 сек)
SELECT title
FROM track
WHERE duration >= 210;

-- 3. Названия сборников, вышедших с 2018 по 2020 год включительно
SELECT title
FROM collection
WHERE release_year BETWEEN 2018 AND 2020;

-- 4. Исполнители, чьё имя состоит из одного слова
SELECT name
FROM artist
WHERE name NOT LIKE '% %';

-- 5. Названия треков, содержащих слово «мой» или «my» (как отдельное слово)
SELECT title
FROM track
WHERE title ~ '(^|\s)([Мм]ой|[Mm][Yy]|MY)(\s|$)';

-- ===================== Задание 3 =====================

-- 1. Количество исполнителей в каждом жанре
SELECT g.name AS genre, COUNT(ag.artist_id) AS artists_count
FROM genre g
JOIN artist_genre ag ON ag.genre_id = g.id
GROUP BY g.id, g.name
ORDER BY g.name;

-- 2. Количество треков, вошедших в альбомы 2019–2020 годов
SELECT COUNT(t.id) AS tracks_count
FROM track t
JOIN album a ON a.id = t.album_id
WHERE a.release_year BETWEEN 2019 AND 2020;

-- 3. Средняя продолжительность треков по каждому альбому
SELECT a.title AS album, ROUND(AVG(t.duration), 1) AS avg_duration
FROM album a
JOIN track t ON t.album_id = a.id
GROUP BY a.id, a.title
ORDER BY a.title;

-- 4. Все исполнители, которые не выпустили альбомы в 2020 году
SELECT name
FROM artist
WHERE id NOT IN (
    SELECT aa.artist_id
    FROM artist_album aa
    JOIN album a ON a.id = aa.album_id
    WHERE a.release_year = 2020
);

-- 5. Названия сборников, в которых присутствует исполнитель «Луна»
SELECT DISTINCT c.title
FROM collection c
JOIN collection_track ct ON ct.collection_id = c.id
JOIN track t             ON t.id = ct.track_id
JOIN artist_album aa     ON aa.album_id = t.album_id
JOIN artist ar           ON ar.id = aa.artist_id
WHERE ar.name = 'Луна';

-- ===================== Задание 4* =====================

-- 1. Названия альбомов, в которых присутствуют исполнители более чем одного жанра
SELECT DISTINCT a.title
FROM album a
JOIN artist_album aa ON aa.album_id = a.id
JOIN artist_genre ag ON ag.artist_id = aa.artist_id
GROUP BY a.id, a.title, aa.artist_id
HAVING COUNT(DISTINCT ag.genre_id) > 1;

-- 2. Названия треков, которые не входят в сборники
SELECT t.title
FROM track t
LEFT JOIN collection_track ct ON ct.track_id = t.id
WHERE ct.track_id IS NULL;

-- 3. Исполнитель(и), написавшие самый короткий трек
SELECT DISTINCT ar.name
FROM artist ar
JOIN artist_album aa ON aa.artist_id = ar.id
JOIN track t         ON t.album_id = aa.album_id
WHERE t.duration = (SELECT MIN(duration) FROM track);

-- 4. Названия альбомов, содержащих наименьшее количество треков
SELECT a.title
FROM album a
JOIN track t ON t.album_id = a.id
GROUP BY a.id, a.title
HAVING COUNT(t.id) = (
    SELECT MIN(cnt)
    FROM (SELECT COUNT(*) AS cnt FROM track GROUP BY album_id) s
);
