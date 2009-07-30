using System;
using System.Data;
using System.Configuration;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Xml.Linq;

public static class Message
{
    private static String errorMessage = null;
    private static String detailedMessage = null;

    public static void setErrorMessage(String message)
    {
        errorMessage = message;
    }

    public static String getErrorMessage()
    {
        return errorMessage;
    }

    public static void setDetailedMessage(String message)
    {
        detailedMessage = message;
    }

    public static String getDetailedMessage()
    {
        return detailedMessage;
    }

    public static void clear()
    {
        errorMessage = null;
        detailedMessage = null;
    }
}
