; Classical map
; Use like
;
; (define add2 (lambda (x) (+ a 2)))
;
; (map add2 (quote (1 2 3 4)))
;
(define map
  (lambda (fun x)
    (cond ((cons? x) (cons (fun (car x)) (map fun (cdr x))))
          ((nil? x) nil)
          (T (fun x))
          )
    )
  )

(define list
 (lambda (a &rest b)
  (cons a
   (cond
    ((cons? b) b)
    (T (cons b nil))))))

(define last
  (lambda (a &rest b)
    (cond
      ((nil? b) a)
      ((not (cons? b)) b)
      (T (d (car b) (cdr b))))))

;; Draft implementation of a dictionary - purely functional, thus a bit
;awkward to use:
;
;Define a dict `my-dict` with initial entry "aha" : 12
;
;(define my-dict (dict-set! NIL "aha" 12))
;
;Add another entry "b" : 34
;
;(set! my-dict (dict-set! my-dict "b" 34))
;
;Yes, since it's purely functional, we only get the return value and must overwrite
;the old dict...
;
;Get some values back:
;
;(dict-get my-dict "aha")
;(dict-get my-dict "b")

(define dict-set! (lambda (dict key value)
                    (cons
                      (cons key value) dict)))

(define dict-get (lambda (dict key)
                   (cond
                     ((nil? dict) nil)
                     (
                      (and
                        (cons? dict)
                        (cons? (car dict))
                        (eq key (car (car dict))))
                      (cdr (car dict)))
                     (T (dict-get (cdr dict))))))

