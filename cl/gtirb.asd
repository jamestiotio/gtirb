(defsystem "gtirb"
    :name "gtirb"
    :author "GrammaTech"
    :licence "MIT"
    :description "Common Lisp library for GTIRB"
    :long-description "A Common Lisp front end to the GrammaTech
  Intermediate Representation for Bianries (GTIRB).  GTIRB is
  serialized using Google's protocol buffers.  This library wraps the
  raw protocol buffer serialization with an more Lispy interface."
    :depends-on (proto gtirb/gtirb)
    :class :package-inferred-system
    :defsystem-depends-on (:asdf-package-system :protobuf)
    :in-order-to ((test-op (test-op "gtirb/test"))))

(defsystem "proto-v0"
    :name "proto-v0"
    :description "Common Lisp interface to (old V0) GTIRB protobuf files"
    :author "GrammaTech"
    :license "MIT"
    :defsystem-depends-on (:protobuf)
    :components
    ((:static-file "README.md")
     ;; See the protobuf defsystem extension for how the gtirb.proto
     ;; file is loaded into Lisp.  https://github.com/brown/protobuf
     (:module proto
              :pathname "../proto-v0/"
              :components
              ((:protobuf-source-file "AuxDataContainer")
               (:protobuf-source-file "CFG")
               (:protobuf-source-file "Section")
               (:protobuf-source-file "Offset")
               (:protobuf-source-file "IR")
               (:protobuf-source-file "ByteMap")
               (:protobuf-source-file "ProxyBlock")
               (:protobuf-source-file "AuxData")
               (:protobuf-source-file "Module")
               (:protobuf-source-file "DataObject")
               (:protobuf-source-file "ImageByteMap")
               (:protobuf-source-file "SymbolicExpression")
               (:protobuf-source-file "Symbol")
               (:protobuf-source-file "Block")))))

(defsystem "proto"
    :name "proto"
    :description "Common Lisp interface to GTIRB protobuf files"
    :author "GrammaTech"
    :license "MIT"
    :defsystem-depends-on (:protobuf)
    :components
    ((:static-file "README.md")
     ;; See the protobuf defsystem extension for how the gtirb.proto
     ;; file is loaded into Lisp.  https://github.com/brown/protobuf
     (:module proto
              :pathname "../proto"
              :components
              ((:protobuf-source-file "AuxData")
               (:protobuf-source-file "ByteInterval")
               (:protobuf-source-file "CFG")
               (:protobuf-source-file "CodeBlock")
               (:protobuf-source-file "DataBlock")
               (:protobuf-source-file "IR")
               (:protobuf-source-file "Module")
               (:protobuf-source-file "ProxyBlock")
               (:protobuf-source-file "Section")
               (:protobuf-source-file "Symbol")
               (:protobuf-source-file "SymbolicExpression")))))

(defsystem "gtirb/update"
    :author "GrammaTech"
    :license "MIT"
    :description "Convert between GTIRB protobuf versions."
    :depends-on (proto-v0 proto)
    :class :package-inferred-system
    :defsystem-depends-on (:asdf-package-system :protobuf)
    :build-operation "asdf:program-op"
    :build-pathname "bin/dump-store"
    :entry-point "gtirb/update:update")

(defsystem "gtirb/test"
    :author "GrammaTech"
    :licence "MIT"
    :description "Test the GTIRB package."
    :perform
    (test-op (o c) (symbol-call :gtirb/test '#:test)))
