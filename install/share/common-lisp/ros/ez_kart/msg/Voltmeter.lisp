; Auto-generated. Do not edit!


(cl:in-package ez_kart-msg)


;//! \htmlinclude Voltmeter.msg.html

(cl:defclass <Voltmeter> (roslisp-msg-protocol:ros-message)
  ((voltage
    :reader voltage
    :initarg :voltage
    :type cl:float
    :initform 0.0))
)

(cl:defclass Voltmeter (<Voltmeter>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <Voltmeter>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'Voltmeter)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name ez_kart-msg:<Voltmeter> is deprecated: use ez_kart-msg:Voltmeter instead.")))

(cl:ensure-generic-function 'voltage-val :lambda-list '(m))
(cl:defmethod voltage-val ((m <Voltmeter>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader ez_kart-msg:voltage-val is deprecated.  Use ez_kart-msg:voltage instead.")
  (voltage m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <Voltmeter>) ostream)
  "Serializes a message object of type '<Voltmeter>"
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'voltage))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <Voltmeter>) istream)
  "Deserializes a message object of type '<Voltmeter>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'voltage) (roslisp-utils:decode-double-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<Voltmeter>)))
  "Returns string type for a message object of type '<Voltmeter>"
  "ez_kart/Voltmeter")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'Voltmeter)))
  "Returns string type for a message object of type 'Voltmeter"
  "ez_kart/Voltmeter")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<Voltmeter>)))
  "Returns md5sum for a message object of type '<Voltmeter>"
  "cd1e97d74e6d797b46bc5a51e820e6ae")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'Voltmeter)))
  "Returns md5sum for a message object of type 'Voltmeter"
  "cd1e97d74e6d797b46bc5a51e820e6ae")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<Voltmeter>)))
  "Returns full string definition for message of type '<Voltmeter>"
  (cl:format cl:nil "float64 voltage~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'Voltmeter)))
  "Returns full string definition for message of type 'Voltmeter"
  (cl:format cl:nil "float64 voltage~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <Voltmeter>))
  (cl:+ 0
     8
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <Voltmeter>))
  "Converts a ROS message object to a list"
  (cl:list 'Voltmeter
    (cl:cons ':voltage (voltage msg))
))
