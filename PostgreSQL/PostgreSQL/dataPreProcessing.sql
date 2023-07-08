--Script to preprocess 10M Records

--Table to store raw data
CREATE TABLE IF NOT EXISTS public.raw_watdiv
(
    subject text,
    property text,
    object text
);

\copy public.raw_watdiv(subject, property, object)
FROM '\\watdiv.10M.nt'
DELIMITER E'\t'
CSV
QUOTE '"'
ESCAPE '"';

--Table to preporcessed data
CREATE TABLE IF NOT EXISTS public.cleaned_watdiv
(
    subject text,
    property text,
    object text
);

--Cleaning and preprocessing script
INSERT INTO public.cleaned_watdiv
SELECT
REVERSE(SUBSTRING(REVERSE(subject) FROM 2 FOR (POSITION('/' IN REVERSE(subject)) - 2 ))) AS subject
,CASE 
	WHEN property LIKE '%#%' 
		THEN LEFT(SUBSTRING(property FROM (POSITION('#' in property)+1)), -1)
	WHEN property LIKE ANY(ARRAY['%http://db.uwaterloo.ca/%', '%purl.org%', '%schema.org%', '%xmlns.com%']) 
		THEN REVERSE(SUBSTRING(REVERSE(property) FROM 2 FOR (position('/' in REVERSE(property)) - 2 )))
	END AS property
,CASE 
	WHEN object LIKE '%http://db.uwaterloo.ca/%' 
		THEN REVERSE(SUBSTRING(REVERSE(object) FROM 4 FOR (POSITION('/' IN REVERSE(object)) - 4 )))
	--ELSE substring(object from 1 for (length(object) - 2)) END as object
	ELSE 
		LEFT(object, -2) END AS object
FROM public.raw_watdiv;

--Dynamic SQL Script to Create multiple tables based on Property Value and insert data based on the property
DO $$
DECLARE 
	property_table_name varchar;
	create_table_stat varchar;
	insert_table_stat varchar;
	--truncate_table_stat varchar;
BEGIN
FOR property_table_name IN (SELECT DISTINCT(Property) FROM public.cleaned_watdiv) 
	LOOP
	--truncate_table_stat := (SELECT format('TRUNCATE TABLE public.%s', property_table_name));
	--EXECUTE truncate_table_stat;
	create_table_stat := (SELECT format('CREATE TABLE IF NOT EXISTS public.%s ( subject text, object text)', property_table_name));
	EXECUTE create_table_stat;
	insert_table_stat := (SELECT format('INSERT INTO public.%s  SELECT subject, object FROM public.cleaned_watdiv WHERE property = ''%s''',property_table_name,property_table_name));
	EXECUTE insert_table_stat;
	END LOOP;
END $$;	

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

--Script to preprocess 100k Records

--Table to store raw data
CREATE TABLE IF NOT EXISTS public.raw_watdiv
(
    subject text,
    property text,
    object text
);

\copy public.raw_watdiv(subject, property, object)
FROM '\\watdiv.100K.txt'
DELIMITER E'\t'
CSV
QUOTE '"'
ESCAPE '"';

--Table to preporcessed data
CREATE TABLE IF NOT EXISTS public.cleaned_watdiv
(
    subject text,
    property text,
    object text
);

--Cleaning and preprocessing script
INSERT INTO public.cleaned_watdiv
SELECT
	TRIM(SUBSTRING(subject,POSITION(':' IN subject)+1)) AS subject,
	TRIM(SUBSTRING(property,POSITION(':' IN property)+1)) AS property,
	TRIM(LEFT(SUBSTRING(object,POSITION(':' IN object)+1),-1)) AS object
FROM public.raw_watdiv;

--Dynamic SQL Script to Create multiple tables based on Property Value and insert data based on the property
DO $$
DECLARE 
	property_table_name varchar;
	create_table_stat varchar;
	insert_table_stat varchar;
	--truncate_table_stat varchar;
BEGIN
FOR property_table_name IN (SELECT DISTINCT(Property) FROM public.cleaned_watdiv) 
	LOOP
	--truncate_table_stat := (SELECT format('TRUNCATE TABLE public.%s', property_table_name));
	--EXECUTE truncate_table_stat;
	create_table_stat := (SELECT format('CREATE TABLE IF NOT EXISTS public.%s ( subject text, object text)', property_table_name));
	EXECUTE create_table_stat;
	insert_table_stat := (SELECT format('INSERT INTO public.%s  SELECT subject, object FROM public.cleaned_watdiv WHERE property = ''%s''',property_table_name,property_table_name));
	EXECUTE insert_table_stat;
	END LOOP;
END $$;	