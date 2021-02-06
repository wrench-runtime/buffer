foreign class Buffer {
  construct new(size){
    create_(size)
  }
  construct fromString(str){
    var size = str.bytes.count+1
    create_(size)
    writeBytes(0,str)
  }
  foreign resize(size)
  foreign toBytes(offset)
  toBytes(){toBytes(0)}
  foreign size
  offset { 0 }
  buffer { this }

  foreign create_(size)
  foreign dispose()
  foreign isDisposed

  foreign writeUint8(offset,val)
  foreign writeUint16(offset,val)
  foreign writeUint32(offset,val)
  foreign writeInt32(offset,val)
  foreign writeInt16(offset,val)
  foreign writeInt8(offset,val)
  foreign writeFloat(offset,val)
  foreign writeDouble(offset,val)
  foreign writeString(offset, val)
  foreign writeBytes(offset, val)

  foreign readUint8(offset)
  foreign readUint16(offset)
  foreign readUint32(offset)
  foreign readInt32(offset)
  foreign readInt16(offset)
  foreign readInt8(offset)
  foreign readFloat(offset)
  foreign readDouble(offset)
  foreign readBytes(offset, length)
  foreign readString(offset)

  foreign copyTo(dst, srcOffset, dstOffset, size)
}

class BufferView {
  offset { _offset }
  size { _size }
  buffer { _buffer }

  construct new(buffer, offset, size){
    init_(buffer, offset, size)
  }
  construct new(buffer, offset){
    init_(buffer, offset, size-offset)
  }
  construct new(buffer){
    init_(buffer, 0, buffer.size)
  }

  init_(buffer, offset, size){
    _buffer = buffer
    if(offset+size > buffer.size){
      Fiber.abort("BufferView out ot bounds")
    }
    _offset = offset
    _size = size
  }

  writeUint8(offset, val) { _buffer.writeUint8(_offset+offset, val) }
  writeUint16(offset, val) { _buffer.writeUint16(_offset+offset, val) }
  writeUint32(offset, val) { _buffer.writeUint32(_offset+offset, val) }
  writeInt32(offset, val) { _buffer.writeInt32(_offset+offset, val) }
  writeInt16(offset, val) { _buffer.writeInt16(_offset+offset, val) }
  writeInt8(offset, val) { _buffer.writeInt8(_offset+offset, val) }
  writeFloat(offset, val) { _buffer.writeFloat(_offset+offset, val) }
  writeDouble(offset, val) { _buffer.writeDouble(_offset+offset, val) }

  readUint8(offset, val) { _buffer.readUint8(_offset+offset) }
  readUint16(offset, val) { _buffer.readUint16(_offset+offset) }
  readUint32(offset, val) { _buffer.readUint32(_offset+offset) }
  readInt32(offset, val) { _buffer.readInt32(_offset+offset) }
  readInt16(offset, val) { _buffer.readInt16(_offset+offset) }
  readInt8(offset, val) { _buffer.readInt8(_offset+offset) }
  readFloat(offset, val) { _buffer.readFloat(_offset+offset) }
  readDouble(offset, val) { _buffer.readDouble(_offset+offset) }
}

class TypeSize {
  static Uint8 { 1 }
  static Int8 { 1 }
  static Uint16 { 2 }
  static Int16 { 2 }
  static Uint32 { 4 }
  static Int32 { 4 }
  static Float { 4 }
  static Double { 8 }
}

class BufferArray is Sequence {

  buffer { _buffer }
  size { _buffer.size }
  offset { _offset }
  count { _buffer.size / _compSize }
  //toList { this.map{|x| t} }

  construct new(buffer, componentSize){
    init_(buffer, componentSize)
  }

  construct fromBuffer(buffer, componentSize){
    init_(buffer, componentSize)
  }

  construct fromList(buffer, componentSize, list){
    init_(buffer, componentSize)
    fill(list)
  }

  init_(buffer, componentSize){
    _offset = 0
    _buffer = buffer
    _compSize = componentSize
    if(_buffer.size % _compSize != 0){ Fiber.abort("Buffer and typed array are not aligned")}
  }

  fill(arr){
    for(i in 0...arr.count){
      this[i] = arr[i]
    }
  }

  iterate(val) { val == null ? 0 : ( val >= count-1 ? false : val+1)  }
  iteratorValue(i) { getValue(i*_compSize) }
  [i] { getValue(i*_compSize) }
  [i]=(v) { setValue(i*_compSize, v) }
}

class Uint8Array is BufferArray {
  construct new(size){ super(Buffer.new(size*TypeSize.Uint8), TypeSize.Uint8) }
  construct fromList(list){ super(Buffer.new(list.count*TypeSize.Uint8), TypeSize.Uint8, list) }
  construct fromBuffer(buffer){ super(buffer, TypeSize.Uint8) }
  getValue(offset) { buffer.readUint8(offset) }
  setValue(offset, val) { buffer.writeUint8(offset, val) }
}

class Uint16Array is BufferArray {
  construct new(size){ super(Buffer.new(size*TypeSize.Uint16), TypeSize.Uint16) }
  construct fromList(list){ super(Buffer.new(list.count*TypeSize.Uint16), TypeSize.Uint16, list) }
  construct fromBuffer(buffer){ super(buffer, TypeSize.Uint16) }
  getValue(offset) { buffer.readUint16(offset) }
  setValue(offset, val) { buffer.writeUint16(offset, val) }
}

class Uint32Array is BufferArray {
  construct new(size){ super(Buffer.new(size*TypeSize.Uint32), TypeSize.Uint32) }
  getValue(offset) { buffer.readUint32(offset) }
  setValue(offset, val) { buffer.writeUint32(offset, val) }
}

class Int8Array is BufferArray {
  construct new(size){ super(Buffer.new(size*TypeSize.Int8), TypeSize.Int8) }
  getValue(offset) { buffer.readInt8(offset) }
  setValue(offset, val) { buffer.writeInt8(offset, val) }
}

class Int16Array is BufferArray {
  construct new(size){ super(Buffer.new(size*TypeSize.Int16), TypeSize.Int16) }
  getValue(offset) { buffer.readInt16(offset) }
  setValue(offset, val) { buffer.writeInt16(offset, val) }
}

class Int32Array is BufferArray {
  construct new(size){ super(Buffer.new(size*TypeSize.Int32), TypeSize.Int32) }
  getValue(offset) { buffer.readInt32(offset) }
  setValue(offset, val) { buffer.writeInt32(offset, val) }
}

class FloatArray is BufferArray {
  construct new(size){ super(Buffer.new(size*TypeSize.Float), TypeSize.Float) }
  construct fromList(list){ super(Buffer.new(list.count*TypeSize.Float), TypeSize.Float, list) }
  getValue(offset) { buffer.readFloat(offset) }
  setValue(offset, val) { buffer.writeFloat(offset, val) }
}

class DoubleArray is BufferArray {
  construct new(size){ super(Buffer.new(size*TypeSize.Double), TypeSize.Double) }
  getValue(offset) { buffer.readDouble(offset) }
  setValue(offset, val) { buffer.writeDouble(offset, val) }
}



