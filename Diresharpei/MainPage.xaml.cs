namespace Diresharpei
{
    using Windows.UI.Xaml.Controls;

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
            Direlibre.Class1 c1 = new Direlibre.Class1();

            c1.Test11();
            c1.Test12();
        }
    }
}
