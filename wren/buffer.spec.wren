import "augur" for Augur, Assert
import "buffer" for Buffer

Augur.describe(Buffer) {
  Augur.it("creates with size"){
    var b = Buffer.new(32)
    Assert.equal(b.size, 32)
  }
}

Augur.run()