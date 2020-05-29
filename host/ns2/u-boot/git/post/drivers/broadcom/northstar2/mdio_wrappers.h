/* PCIE-A */
void mdio_write_pcie_a(uint32_t reg, uint16_t v);
uint16_t mdio_read_pcie_a(uint32_t reg);
/* PCIE-B */
void mdio_write_pcie_b(uint32_t reg, uint16_t v);
uint16_t mdio_read_pcie_b(uint32_t reg);
/* USB3 */
void mdio_write_usb30(uint32_t reg, uint16_t v);
uint16_t mdio_read_usb30(uint32_t reg);
/* USB2 HS0 */
void mdio_write_usb20_hs0(uint32_t reg, uint16_t v);
uint16_t mdio_read_usb20_hs0(uint32_t reg);
/* USB2 HS1 */
void mdio_write_usb20_hs1(uint32_t reg, uint16_t v);
uint16_t mdio_read_usb20_hs1(uint32_t reg);
/* USB2 DRD */
void mdio_write_usb20_drd(uint32_t reg, uint16_t v);
uint16_t mdio_read_usb20_drd(uint32_t reg);
/* SATA 0 */
void mdio_write_sata_port0(uint32_t reg, uint16_t v);
uint16_t mdio_read_sata_port0(uint32_t reg);
/* SATA 1 */
void mdio_write_sata_port1(uint32_t reg, uint16_t v);
uint16_t mdio_read_sata_port1(uint32_t reg);
/* GPHY */
void mdio_write_extphy(uint32_t reg, uint16_t v);
uint16_t mdio_read_extphy(uint32_t reg);
/* TSC-E */
void mdio_write_eagle(uint32_t reg, uint16_t v);
uint16_t mdio_read_eagle(uint32_t reg);

