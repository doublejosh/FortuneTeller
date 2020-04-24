import React from 'react'
import { useSelector } from 'react-redux'
import { useFirebaseConnect, isEmpty } from 'react-redux-firebase'
import Fortune from '../Dashboard/Fortune'
import { css } from 'glamor'
import { Link } from 'react-router-dom'

export default () => {
	useFirebaseConnect(['fortunes'])
	let fortunes = useSelector(state => state.firebase.data.fortunes)

	const list = Object.keys(fortunes || {})
		.filter(f => fortunes[f])
		.map(f => {
			return {
				...fortunes[f],
				key: f,
				score:
					(fortunes[f].rank
						? (fortunes[f].rank.keep ? fortunes[f].rank.keep : 0) /
						  (fortunes[f].rank.total ? fortunes[f].rank.total : 1)
						: 0) * 100,
			}
		})
		// Key, new first.
		.sort((a, b) => {
			if (a.key.toString() < b.key.toString()) return -1
			if (a.key.toString() > b.key.toString()) return 1
			return 0
		})
		// Total votes.
		.sort((a, b) => {
			if (a.rank && a.rank.total) {
				if (b.rank && b.rank.total) {
					return b.rank.total - a.rank.total
				}
				return 1
			}
			return -1
		})
		// Final score.
		.sort((a, b) => b.score - a.score)

	const total = !isEmpty(list)
		? list.reduce((total, f) => (f.rank && f.rank.total ? f.rank.total + total : total), 0)
		: 0

	return (
		<section {...css({ margin: '1rem' })}>
			{fortunes ? (
				<React.Fragment>
					<h3
						{...css({
							margin: 0,
							fontSize: '1.25rem',
							color: 'rgba(0,0,0, .5)',
							textAlign: 'right',
							textTransform: 'uppercase',
						})}>
						Approval
					</h3>
					<ul {...css({ paddingLeft: '0', listStyle: 'none', marginTop: '.5rem' })}>
						{list.map((f, i) => (
							<Fortune key={i} f={f} hideVotes={true} />
						))}
					</ul>
					<div
						{...css({
							textAlign: 'right',
							marginTop: '1rem',
							fontSize: '3rem',
							lineHeight: '3rem',
							color: 'rgba(0, 0, 0, 0.6)',
						})}>
						<h2 {...css({ marginBottom: 0 })}>{total}</h2>
						<span>votes</span>
					</div>
				</React.Fragment>
			) : (
				<h2
					{...css({
						gridArea: 'message',
						display: 'flex',
						alignItems: 'center',
						justifyContent: 'center',
						animation: 'blinkingText 1.2s infinite',
					})}>
					Loading...
				</h2>
			)}
			<h2 {...css({ textAlign: 'center', marginTop: '5rem' })}>
				<Link to="/findings" {...css({ color: '#FFF' })}>
					Findings
				</Link>
			</h2>
		</section>
	)
}
