const char indexcss[] PROGMEM = R"=====(
* {
  font-family: Arial, Helvetica, sans-serif;
}
html {
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  font-family: 'Dosis', sans-serif;
  line-height: 1.6;
  color: white;
  background: #000000;
}
table {
  color: white;
  border-collapse: collapse;
  width: 90%;
}
th {
  white-space: nowrap;
  border-right: 1px solid lightgrey;
  border-left: 1px solid lightgrey;
  vertical-align: bottom;
}
td {
  white-space: nowrap;
  border-top: 1px solid lightgrey;
  border-bottom: 1px solid lightgrey;
  border-right: 1px solid lightgrey;
  border-left: 1px solid lightgrey;
  vertical-align: bottom;
}
tfoot {
  font-size: 12px;
  color: darkblue;
}
input {
   padding-top: 3px;
   padding-bottom: 2px;
   font-size: 12pt;
}
input::file-selector-button {
	width: 150px;
	height: 40px;
	font-size: 14px;
	cursor: pointer;
	background-color: lightsteelblue;
  color: white;
}
input::file-selector-button:hover {
	width: 150px;
	height: 40px;
	font-size: 14px;
	cursor: pointer;
	background-color: steelblue;
}
[type=submit] {
	height: 40px;
	min-width: 70px;
}
.settingDiv {
  white-space: nowrap;
}
.outer-div {
  text-align: center;
  background-color: #e6ffff
}
.inner-div {
  display: inline-block;
}
.container-card {
  padding: 50;
  margin: 50;
  list-style: none;
  flex-direction: row;
  display: -webkit-box;
  display: -moz-box;
  display: -ms-flexbox;
  display: -webkit-flex;
  display: flex;
  -webkit-flex-flow: row wrap;
  flex-flow: row wrap;
  justify-content: space-around;
}
.container-box {
  padding: 10px;
  margin: 10px;
  list-style: none;
  flex-direction: column;
  display: -webkit-box;
  display: -moz-box;
  display: -ms-flexbox;
  display: -webkit-flex;
  display: flex;
  -webkit-flex-flow: column wrap;
  flex-flow: row wrap;
  justify-content: flex-start;
  margin-top: 10px;
}
.nav-container {
  list-style: none;
  margin: 0;
  background: darkgrey;
  display: -webkit-box;
  display: -moz-box;
  display: -ms-flexbox;
  display: -webkit-flex;
  display: flex-start;
  -webkit-flex-flow: row wrap;
  flex-flow: row wrap;
  min-height: 24px;
}
.nav-img {
  top: 1px;
  display: inline-block;
  width: 40px;
  height: 40px;
}
.nav-item {
  display: inline-grid;
  font-size: 16px;
  padding: 5px 5px 5px 5px;
  height: 48px;
  width: 110px;
  border: none;
  color: white;
  background: lightgrey;
}
.nav-left {
  flex: 2 1 0;
  text-align: center;
}
.nav-right {
  flex: 1 1 0;
  text-align: right;
  width: 200px;
}
.nav-container a:hover {
  color: white;
  background: steelblue; /* only for FSexplorer - Rest does not work */
}
.nav-clock {
  top: 1px;
  color: black;
  float: right;
  font-size: small;
  font-weight:bold;
  text-align: right;
  background: white;
  width: 200px;
  padding-right: 10px;
  background: #e6ffff;
}
.tabButton {
  background-color: lightsteelblue;
  color: white;
  text-decoration: none;
}
.tabButton a:hover {
  background-color: steelblue;
  color: white;
  text-decoration: none;
}
.tabButtonred {
	background-color: darkred;
	color: white;
  text-decoration: none;
}
.tabButtonred:hover{
	background-color: red;
	color: white;
  text-decoration: none;
}
.header h1 span {
  position: relative;
  top: 1px;
  left: 10px;
}
.bottom {
  position: fixed;
  font-size: small;
  color: gray;
  bottom:0;
}
.right-0  {right: 0; padding-right: 10px; }
.left-0   {left:  0; padding-left:  10px; }
.headerrow {
  vertical-align:bottom;
  background-color: grey;
}
.headercolumnbig{
  font-size: x-large;
  font-weight: bold;
  float: left;
}
.headercolumn{
  font-size: smaller;
  float: left;
  width: 175px;
}
.headercolumnshort{
  font-size: smaller;
  float: left;
}
.headerrow:after {
  content: "";
  display: table;
  clear: both;
}
@media screen and (max-width: 600px) {
  .column {
    width: 100%;
  }
}
.devinforow:after {
  content: "";
  display: table;
  clear: both;
  background: lightgrey;
}
button { width: 100px; }
.Modbusmonrow {
  display: table-row;
}
.Modbusmoncolumn1 {
  display: table-cell;
  float: left;
  width: 160px;
}
.Modbusmoncolumn2 {
  display: table-cell;
  float: left;
  text-align: right;
  width: 100px;
}
.Modbusmoncolumn3 {
  display: table-cell;
  float: left;
  width: 45px;
  margin-left: 2px;
  text-align: right;
}
)=====";