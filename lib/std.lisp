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
