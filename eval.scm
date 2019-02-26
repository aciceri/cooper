#!/bin/guile \
-e main -s
!#

(use-modules (ice-9 format) (ice-9 eval-string))

(define (div a b)
  (if (= (remainder a b) 0) #t #f))

(define true #t)

(define false #f)

(define (main args)
  (let ((str (cadr args)))
    (format #t
            "\nInput: ~s\nEvaluated: ~s\n"
            str
            (if (eval-string str) "true" "false"))))
