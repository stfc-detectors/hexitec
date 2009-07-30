<%@ Page Language="C#" AutoEventWireup="true" CodeFile="Error.aspx.cs" Inherits="Error" %>

<%@ Register assembly="netchartdir" namespace="ChartDirector" tagprefix="chart" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head id="Head1" runat="server">
    <title>PowerPredict Error</title>
    <link href="power.css" rel="stylesheet" type="text/css" />
</head>
<body class="twoColHybLt">
    <form id="form1" runat="server">
   <div  class="twoColHybLt" id="container" runat="server">
      <div id="header">
         <p><img src="images/Header.jpg" width="711" height="107" /></p>
      </div>
      <div class="twoColHybLt" id="sidebar1">
         <ul>
            <li><a href="#">Home</a></li>
            <li><a href="Index.aspx">Wind Farms</a></li>
            <li><a href="Chart.aspx">Total Power</a></li>
            <li><a href="#">Events</a></li>
            <li><a href="#">Technology</a></li>
            <li><a href="#">Contact Us</a></li>
         </ul>
         <!-- end #sidebar1 -->
      </div>
      <div id="mainContent" style="font-size:small;color:Black">
          <asp:Label ID="ErrorMessage" runat="server" Text="The following error has occured : "></asp:Label>
          <asp:Label ID="ErrorMessageText" runat="server"></asp:Label>
          <br />
          <asp:Label ID="DetailedMessage" runat="server" Text=""></asp:Label>
          <asp:Label ID="DetailedMessageText" runat="server" Text=""></asp:Label>
      </div>
   </div>
</form>
</body>
</html>