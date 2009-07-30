using System;
using System.Collections;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Xml.Linq;

public partial class Error : System.Web.UI.Page
{
    protected void Page_Load(object sender, EventArgs e)
    {
        String message;

        ErrorMessageText.Text = Message.getErrorMessage();

        if ((message = Message.getDetailedMessage()) != null)
        {
            DetailedMessage.Text = "Detailed information : ";
            DetailedMessageText.Text = message;
        }

        Message.clear();
    }
}
