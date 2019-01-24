//-----------------------------------------------------------------------------
/** @file pentobi/qml/Controls.js
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

// Helper function to add a mnemonic without using an ampersand in the
// translatable MenuItem text or in the Action text.
// Ampersands in texts cause problems for translation with Weblate because
// Weblate does not support searching strings containing ampersands.
// Also, there seems to be a bug in Qt that in some cases registers mnemonics
// in Action texts used in a MenuItem as global shortcuts (last occurred with
// Qt 5.11.1)
function addMnemonic(text, mnemonic) {
    if (! isDesktop || mnemonic === "")
        return text
    mnemonic = mnemonic.toUpperCase()
    var textUpper = text.toUpperCase()
    var pos = textUpper.indexOf(mnemonic)
    // Prefer beginning of word
    while (pos >= 0 && pos < textUpper.length) {
        if (pos === 0 || textUpper.charAt(pos - 1) === " ")
            break
        pos = textUpper.indexOf(mnemonic, pos + 1)
    }
    if (pos < 0 || pos >= textUpper.length)
        pos = textUpper.indexOf(mnemonic)
    if (pos >= 0)
        return text.substring(0, pos) + "&" + text.substring(pos)
    return text + " (&" + mnemonic + ")"
}
