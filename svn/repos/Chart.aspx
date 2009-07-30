<%@ Page Language="C#" AutoEventWireup="true" CodeFile="Chart.aspx.cs" Inherits="Chart" %>

<%@ Register assembly="netchartdir" namespace="ChartDirector" tagprefix="chart" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title>PowerPredict Wind Farm Data</title>
    <link href="power.css" rel="stylesheet" type="text/css" />
</head>
<body class="twoColHybLt">
   <div  class="twoColHybLt" id="container" runat="server">
      <div id="header">
         <p><img src="images/Header.jpg" width="711" height="107" /></p>
      </div>
      <div class="twoColHybLt" id="sidebar1">
         <ul>
            <li><a href="index.htm">Home</a></li>
            <li><a href="events.htm">Events</a></li>
            <li><a href="technology.htm">Technology</a></li>
            <li><a href="contact.htm">Contact Us</a></li>
         </ul>
         <!-- end #sidebar1 -->
      </div>
      <div id="mainContent" style="font-size:small;color:Black">
         <chart:WebChartViewer ID="WebChartViewer1" runat="server" />
      </div>
   </div>
</body>
</html>