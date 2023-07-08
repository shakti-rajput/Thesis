SET enable_nestloop TO OFF;
SET enable_mergejoin TO OFF;
SET enable_hashjoin TO ON;
SET enable_parallel_hash TO ON;

SELECT DISTINCT fo.subject,fo.object
FROM follows fo 
INNER JOIN friendOf fr ON fr.subject = fo.object
INNER JOIN likes l ON l.subject = fr.object
INNER JOIN hasReview hr ON hr.subject = l.object;