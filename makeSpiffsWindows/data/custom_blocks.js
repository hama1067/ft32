/*
 *
 */
 Blockly.Blocks['start'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("Start");
    this.appendStatementInput("state");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
 Blockly.BlockSvg.START_HAT = true;
  }
};

/*
 *
 */
Blockly.Blocks['wait'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("wait")
        .appendField(new Blockly.FieldNumber(0, 0, 255, 0.01), "seconds")
        .appendField("seconds");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(260);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

/*
 *
 */
Blockly.Blocks['motor'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("Set Motor")
        .appendField(new Blockly.FieldDropdown([["M0","1"], ["M1","2"], ["M2","3"], ["M3","4"]]), "motorNumber")
        .appendField(" direction")
        .appendField(new Blockly.FieldDropdown([["left","0"], ["right","1"]]), "motorDirection")
        .appendField("speed")
        .appendField(new Blockly.FieldDropdown([["0","0"], ["1","1"], ["2","2"], ["3","3"], ["4","4"], ["5","5"], ["6","6"], ["7","7"], ["8","8"]]), "motorSpeed");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(290);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

/*
 *
 */
Blockly.Blocks['lamp'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("Set")
        .appendField(new Blockly.FieldDropdown([["L0","1"], ["L1","2"], ["L2","3"], ["L3","4"]]), "lampNumber")
        .appendField("to")
        .appendField(new Blockly.FieldDropdown([["0","1"], ["1","2"], ["2","3"], ["3","4"], ["4","5"], ["5","6"], ["6","7"], ["7","8"]]), "state");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(290);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

/*
 * Freifeld
 */
 /*Blockly.Blocks['digital_logic_compare'] = {
  init: function() {
    this.appendDummyInput()
        .appendField(new Blockly.FieldNumber(0, 0, 7, 1), "A")
        .appendField(new Blockly.FieldTextInput("="), "OP")
        .appendField(new Blockly.FieldNumber(0, 0, 1, 1), "B");
    this.setInputsInline(true);
    this.setOutput(true, "Boolean");
    this.setColour(0);
 this.setTooltip("Digital Compare");
 this.setHelpUrl("");
  }
};*/

/*
 *
 */
Blockly.Blocks['analog_logic_compare'] = {
  init: function() {
    this.appendDummyInput()
        .appendField(new Blockly.FieldDropdown([["Port 0","0"], ["Port 1","1"], ["Port 2","2"], ["Port 3","3"], ["Port 4","4"], ["Port 5","5"], ["Port 6","6"], ["Port 7","7"]]), "A")
        .appendField(new Blockly.FieldDropdown([["=","EQ"], ["<","LT"], [">","GT"]]), "OP")
        .appendField(new Blockly.FieldNumber(0, 0, 127, 1), "B");
    this.setInputsInline(true);
    this.setOutput(true, "Boolean");
    this.setColour(0);
 this.setTooltip("Analog Compare");
 this.setHelpUrl("");
  }
};

/*
 * Dropdown
 */
Blockly.Blocks['digital_logic_compare'] = {
  init: function() {
    this.appendDummyInput()
        .appendField(new Blockly.FieldDropdown([["Port 0","0"], ["Port 1","1"], ["Port 2","2"], ["Port 3","3"], ["Port 4","4"], ["Port 5","5"], ["Port 6","6"], ["Port 7","7"]]), "A")
        .appendField(new Blockly.FieldTextInput("="), "OP")
        .appendField(new Blockly.FieldDropdown([["0","0"], ["1","1"]]), "B");
    this.setInputsInline(true);
    this.setOutput(true, "Boolean");
    this.setColour(0);
 this.setTooltip("Digital Compare");
 this.setHelpUrl("");
  }
};

/*
 *
 */

Blockly.Blocks['controls_if1'] = {
  init: function() {
    this.appendValueInput("IF0")
        .setCheck("Boolean")
        .appendField("if");
    this.appendStatementInput("DO0")
        .setCheck(null)
        .appendField("do");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

/*
 *
 */
 
 Blockly.Blocks['controls_whileuntil1'] = {
  init: function() {
    this.appendValueInput("BOOL")
        .setCheck("Boolean")
        .appendField("repeat while");
    this.appendStatementInput("DO")
        .setCheck(null)
        .appendField("do");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(120);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};