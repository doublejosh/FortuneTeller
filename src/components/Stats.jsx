import React, { useMemo } from 'react'
import { css } from 'glamor'

let style = css({
	fontSize: '2.5rem',
	margin: '0 0 24rem 0',
	color: 'rgba(0, 0, 0, 0.6)',
	textAlign: 'center',
	'@media (min-width: 600px)': {
		textAlign: 'left',
		margin: '0 0 6rem 0',
	},
})

export default props => {
	const findTotal = fortunes => {
		let votes = 0,
			shown = 0
		if (fortunes) {
			fortunes.forEach(f => {
				votes += f.votes
				shown += f.shown
			})
		}
		return { votes: votes, shown: shown }
	}

	const totals = useMemo(() => findTotal(props.fortunes), [props.fortunes])

	return (
		<section className={style}>
			{Math.round(
				(parseInt(totals.votes) / parseInt(totals.shown)) * 100
			)}
			% "Accurate" <br />({totals.votes}/{totals.shown})
		</section>
	)
}
